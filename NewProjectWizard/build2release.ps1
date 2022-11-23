echo "Copying Wizard Executable to release"
cp "bin/x64/release/net6.0-windows/*.exe" ../release
echo "Copying Wizard Library to release"
cp "bin/x64/release/net6.0-windows/*.dll" ../release
echo "Copying Wizard JSON crap"
cp "bin/x64/release/net6.0-windows/*.json" ../release