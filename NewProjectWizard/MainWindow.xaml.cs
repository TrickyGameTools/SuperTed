// Lic:
// SuperTed New Project Wizard
// Main Window
// 
// 
// 
// (c) Jeroen P. Broks, 2022
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
// Version: 22.11.23
// EndLic

using System;
using System.Collections.Generic;
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

	/// <summary>
	/// Interaction logic for MainWindow.xaml
	/// </summary>
	public partial class MainWindow : Window {
		static GINIE GlobData = GINIE.FromFileAuto(Dirry.C("$AppSupport$/SuperTed.ini"));
		static GINIE PrjData = GINIE.Empty();
		static Dictionary<TextBox, string> TextBoxLink = new Dictionary<TextBox, string>();
		static List<TextBox> TextBoxRequired=new List<TextBox>();

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

		private void CreateProject(object sender, RoutedEventArgs e) {

		}

}