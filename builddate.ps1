param(
	[string]$pd = "*nothing*"
)
function private:builddate($d){
        $date = Get-Date
        $cpp = '

        #pragma once
        #define BPP_BuildDate "' + $date + '"
        '

        echo $cpp > $d/builddate.hpp
}


if ($pd -eq "*nothing*") {
	Write-Output "No dir to output to!"
} else {
	builddate $pd
}
