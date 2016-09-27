for /L %%i in (1,1,1000) do (
    echo any-key | brpc.exe api-test
)