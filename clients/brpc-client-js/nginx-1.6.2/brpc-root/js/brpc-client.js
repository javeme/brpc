(function(){
    //hook ajax.success
    function hookAjaxSuccess(request) {    
        var origAjaxSuccess = request.success;
        request.success = function() {
            var status = this.getStatus();
            if (status == 0)
                return false;
            //apply([thisObj[,argArray]]); call([thisObj[,arg1[, arg2[,   [,.argN]]]]]) 
            return origAjaxSuccess.call(request);
        }
        return request;
    }

    //rpc client for js
    var BRpcClient = Class.create({
        initialize: function(url, username, passwd, options) {
            this.url = url;
            this.username = username;
            this.passwd = passwd;
            this.authToken = null;
            
            this.login(username, passwd, options);
        },
        //send http post
        doPost: function(url, data, options) {
            var ajaxOptions = {
                //method: 'get',
                //parameters: {para1: '12', para2: 12},
                //encoding: 'GBK',
                method: 'post',
                contentType: 'application/json',
                postBody: Object.toJSON(data)
            };
            Object.extend(ajaxOptions, options);
            var request = new Ajax.Request(url, ajaxOptions);
            hookAjaxSuccess(request);
            return request;
        },
        //invoke method
        call: function(method, args, options) {
            var body = {method: method,
                        args: args,        
                        authToken: this.authToken,
                        needReturn: true,
                        status: 10,
                        version: '1.0',
                        owner: ''
                       };
                       
            var url = this.url + "?id=" + randomString(16); 
            this.doPost(url, body, options);
        },
        //login
        login: function(username, passwd, options) {
            var body = {method: 'login',
                        args: [username, passwd],        
                        authToken: this.authToken,
                        needReturn: true,
                        status: 11,
                        version: '1.0',
                        owner: ''
                       };
            var that = this;
            var onSuccess = options['onSuccess'] || function(t){};
            options['onSuccess'] = function(transport) {
                //var response = transport.responseText || "no response text";
                var response = transport.responseJSON;
                that.authToken = response.authToken;
                onSuccess(transport);
            };
            this.doPost(this.url, body, options);
        },
        //logout
        logout: function(options) {
            var body = {method: 'logout',
                        args: null,        
                        authToken: this.authToken,
                        needReturn: true,
                        status: 12,
                        version: '1.0',
                        owner: ''
                       };
            var that = this;
            var onSuccess = options['onSuccess'] || function(t){};
            options['onSuccess'] = function(transport) {
                var response = transport.responseJSON;
                that.authToken = null;
                onSuccess(transport);
            };
            this.doPost(this.url, body, options);
        },
        //is login
        hasLogin: function() {
            return this.authToken != null;
        }
    });
    
    //rpc api for js
    var BRpcApi = Class.create({
        initialize: function(serviceName, username, passwd, options, parent) {
            this.serviceName = serviceName;
            this.parent = parent;
            if (!parent)
                this.client = new BRpcClient('/rpcservice', username, passwd, options);
        },
        addSubService: function(name) {
            this[name] = new BRpcApi(name, null, null, null, this);
        },
        getSubService: function(name) {
            return this[name];
        },
        getClient: function() {
            if (!this.parent)
                return this.client;
            return this.parent.getClient();
        },
        getServiceName: function() {
            if (this.parent)
                return this.parent.getServiceName() + '.' + this.serviceName;
            else
                return this.serviceName;
        },
        getServiceMethodName: function(method) {
            return this.getServiceName() + '.' + method;
        },
        /*
        invoke method
        <str> method:
            method name to invoke
        <list> args:
            args of method to invoke
        <map> options: 
            1.options such as encoding/timeout; 
            2.callbacks such as onSuccess/onFailure/onException.
        */
        call: function(method, args, options) {
            var name = this.getServiceMethodName(method);
            var client = this.getClient();
            return client.call(name, args, options);
        },
        ping: function(options){
            /*this.call('ping', [], {
                onSuccess: function(transport){
                    var response = transport.responseJSON;
                    if (onResult)
                        onResult(response);
                },
                onFailure: function(transport){
                    if (onFailure)
                        onFailure(transport);
                },
                onException: function(reqeust, exeption){
                    if (onException)
                        onException(exeption);
                }
            });*/
            this.call('ping', [], options);
        },
        echo: function(str, options){
            this.call('echo', [str], options);
        },
        help: function(options){
            this.call('help', [], options);
        },
        listMethods: function(options){
            this.call('listMethods', [], options);
        },
        listServices: function(options){
            this.call('listServices', [], options);
        },
        listVars: function(options){
            this.call('listVars', [], options);
        },
        signatureOf: function(methodName, options){
            this.call('signatureOf', [methodName], options);
        },
        typeOfVar: function(varName, options){
            this.call('typeOfVar', [varName], options);
        }
    });
    
    window.RpcClient = BRpcClient;
    window.RpcApi = BRpcApi;
})();
