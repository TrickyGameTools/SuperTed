param(
	[string]$pd = "*nothing*"
)
function private:builddate($d){
        $date = Get-Date
        $cpp = '

        #pragma once
        #define SuperTed_BuildDate "' + $date + '"
        '

        echo $cpp > $d/builddate.hpp

	echo "Build date for '$d' set to $date"
}


if ($pd -eq "*nothing*") {
	Write-Output "No dir to output to!"
} else {
	builddate $pd
}
