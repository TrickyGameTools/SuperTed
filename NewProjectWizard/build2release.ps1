echo "Copying Wizard Executable to release"
cp "bin/x64/release/net6.0-windows/*.exe" ../release
echo "Copying Wizard Library to release"
cp "bin/x64/release/net6.0-windows/*.dll" ../release
echo "Copying Wizard JSON crap"
cp "bin/x64/release/net6.0-windows/*.json" ../release

# Really. I wish there was a better way to do this (like how you can do it with C and C++) but I guess this 
# is the best I can do with a C# project