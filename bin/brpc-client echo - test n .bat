for /L %%i in (1,1,1000) do (
    echo next | brpc.exe client system.echo "\"hello brpc, this is %%i count\""    
)