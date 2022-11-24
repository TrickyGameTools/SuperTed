# Build JCR

function private:builddate{                           
        $date = Get-Date                              
        $ginie = '# Identification data 

[Build]
Date=' + $date + '

[ID]
For=SuperTed
Sig=fbeb79a1

'                                             
                                                      
        echo $ginie > ID/ID.ini
}                                                     

pushd
cd JCR
builddate
del ../Release/SuperTed.JCR
NJCR add ../Release/SuperTed.JCR  -fc zlib -cm zlib
popd
