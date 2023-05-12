// Lic:
// SuperTed New Project Wizard
// Main Window
// 
// 
// 
// (c) Jeroen P. Broks, 2022, 2023
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// 
// Please note that some references to data like pictures or audio, do not automatically
// fall under this licenses. Mostly this is noted in the respective files.
// 
// Version: 23.05.11
// EndLic

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using TrickyUnits;

namespace NewProjectWizard {

	enum ft { File,Directory};

	internal struct DirButton {
		internal ft Type;
		internal TextBox TB;

		internal DirButton(TextBox tB, ft type = ft.Directory) {
			Type = type;
			TB = tB;
		}

	}

	/// <summary>
	/// Interaction logic for MainWindow.xaml
	/// </summary>
	public partial class MainWindow : Window {
		static GINIE GlobData = GINIE.FromFileAuto(Dirry.C("$AppSupport$/SuperTed.ini"));
		static GINIE PrjData = GINIE.Empty();
		static Dictionary<TextBox, string> TextBoxLink = new Dictionary<TextBox, string>();
		static List<TextBox> TextBoxRequired=new List<TextBox>();
		static Dictionary<Button,DirButton> DirButtons=new Dictionary<Button,DirButton>();
		static Dictionary<TextBox,string> MultiTex=new Dictionary<TextBox,string>();

		static string ProjectsDir {
			get {
				if (GlobData["Directories", "Projects"] == "") {
					Confirm.Annoy("I need a directory where the SuperTed project data will be stored before I can create any project at all. Please pick one!");
					var A = FFS.RequestDir();
					if (A=="") {
						Confirm.Annoy("Sorry! No directory, no projects!");
						Environment.Exit(123);
						return "";
					}
					GlobData["Directories", "Projects"] = A.Replace('\\','/');
				}
				return Dirry.AD(GlobData["Directories", "Projects"]);
			}
		}

		public MainWindow() {
			Dirry.InitAltDrives();
			InitializeComponent();
			Title = $"SuperTed Project Creation Wizard - Build: {BuildDate.sBuildDate}";
			if (GlobData["Sys", "Creation"] == "") GlobData["Sys", "Creation"] = $"{DateTime.Now}";
			ProjectDirectory.Text = ProjectsDir;
			TextBoxLink[ProjectName] = "Project;Name";
			TextBoxRequired.Add(ProjectName);
			TextBoxLink[DirMap] = "Maps";
			TextBoxRequired.Add(DirMap);
			DirButtons[DirMapSelect] = new DirButton(DirMap);
			TextBoxLink[DirTex] = "Textures";
			TextBoxRequired.Add(DirTex);
			DirButtons[DirTexSelect] = new DirButton(DirTex);
			TextBoxRequired.Add(LLayers);
			MultiTex[LLayers] = "Layers";
			MultiTex[LZones] = "Zones";
			TextBoxLink[Width] = "Grid;Width"; Width.Text = "32";
			TextBoxLink[Height] = "Grid;Height"; Height.Text = "32";
			TextBoxRequired.Add(Width);
			TextBoxRequired.Add(Height);
			MultiTex[ScriptArea] = "Script;Area";
			MultiTex[ScriptSpot] = "Script;Spot";
			CanCreate();
		}

		void CanCreate() {
			bool _CanCreate = true;
			foreach (var T in TextBoxRequired) _CanCreate = _CanCreate && T.Text != "";
			Create.IsEnabled = _CanCreate;
		}

		private void TBChange(object sender, TextChangedEventArgs e) {
			var TB=(TextBox)sender; ;
			if (!TextBoxLink.ContainsKey(TB)) {                
				Confirm.Annoy("Text box not properly linked!\nAre you using complete code?\nPlease notify Jeroen P. Broks!", "ERROR!", System.Windows.Forms.MessageBoxIcon.Error);
			} else {
				var t = TextBoxLink[TB].Split(';');
				string c = "", k = "";
				switch (t.Length) {
					case 0:
						Confirm.Annoy("Textbox linkup syntax error!\nAre you using complete/stable code?\nPlease notify Jeroen P. Broks!", "ERROR!", System.Windows.Forms.MessageBoxIcon.Error);
						return;
					case 1:
						c = "Directory";
						k = t[0];
						break;
					default:
						c = t[0];
						k = t[1];
						break;
				}
				PrjData[c, k] = TB.Text;
			}
			CanCreate();
		}

		void NoChange(object sender, TextChangedEventArgs e) => CanCreate();

		void UpdateMultiTex() {
			foreach(var I in MultiTex) {
				var t = I.Value.Split(';');
				string c = "", k = "";
				switch (t.Length) {
					case 0:
						Confirm.Annoy("Textbox MultiTex linkup syntax error!\nAre you using complete/stable code?\nPlease notify Jeroen P. Broks!", "ERROR!", System.Windows.Forms.MessageBoxIcon.Error);
						return;
					case 1:
						c = "Layers";
						k = t[0];
						break;
					default:
						c = t[0];
						k = t[1];
						break;
				}
				PrjData.List(c,k).Clear();
				foreach(var item in I.Key.Text.Split(';')) 
					PrjData.ListAdd(c, k, item.Trim());
			}
		}
		

		private void CreateProject(object sender, RoutedEventArgs e) {
			try {
				if (qstr.ToInt(Width.Text) < 4) { Confirm.Annoy("Width must be at least 4"); return; }
				if (qstr.ToInt(Height.Text) < 4) { Confirm.Annoy("Height must be at least 4"); return; }
				UpdateMultiTex();
				var Script = new StringBuilder($"// Generated by the SuperTed Wizard on {DateTime.Now}\n\n Group SuperTedCallBack\n\n");
				foreach (var m in PrjData.List("Script", "Area")) Script.Append($"\tVoid AREA_{m}(Int X, Int Y, Int W, Int H)\n\t\t// Code goes here\n\tEnd\n\n");
				foreach (var m in PrjData.List("Script", "Spot")) Script.Append($"\tVoid SPOT_{m}(Int X, Int Y)\n\t\t//Code goes here\n\tEnd\n\n");
				Script.Append("\n\n\tConstructor\n\t\t// Any init code goes here\n\tEnd\n\nEnd");
				var Out = $"{ProjectDirectory.Text}/{ProjectName.Text}";
				if (Directory.Exists(Out)) throw new Exception($"Directory '{Out}' already exists! Please pick a different name for your project!");
				if (File.Exists(Out)) throw new Exception($"There is a file named '{Out}' in existence! Please delete it as this could spook things up drammatically!");
				Directory.CreateDirectory(Out);
				PrjData.SaveSource($"{Out}/{ProjectName.Text}.ini");
				QuickStream.SaveString($"{Out}/{ProjectName.Text}.neil", Script);
				Confirm.Annoy($"Project {ProjectName.Text} has been created!");
				Environment.Exit(0);
			} catch (Exception ex) {
				Confirm.Annoy($"{ex.Message}", "Error in project creation", System.Windows.Forms.MessageBoxIcon.Error);
			}
		}

		private void DirButton(object sender, RoutedEventArgs e) {
			var DB = (Button)sender;
			if (!DirButtons.ContainsKey(DB)) {
				Confirm.Annoy("Request button has not been linked\nPlease notify Jeroen P. Broks!", "Error!", System.Windows.Forms.MessageBoxIcon.Error);
				return;
			}
			var BFld = DirButtons[DB];
			switch (BFld.Type) {
				case ft.Directory:
					BFld.TB.Text = FFS.RequestDir();
					break;
				case ft.File:
					BFld.TB.Text = FFS.RequestFile();
					break;
				default:
					Confirm.Annoy($"Unknown filesystem type({BFld.Type})\nPlease notify Jeroen P. Broks!", "Error!", System.Windows.Forms.MessageBoxIcon.Error);
					return;
			}
		}

		private void AuthorSet(object sender, RoutedEventArgs e) {
			PrjData["Author", "Textures"] = $"{Author.IsChecked}";
		}
	}
}