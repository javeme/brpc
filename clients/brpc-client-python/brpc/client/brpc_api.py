'''
Created on 2015-6-7

@author: Administrator
'''
from brpc.client import brpc_client


class BRpcApi(object):
    '''brpc api for python'''
    def __init__(self, url, service, username, passwd, parent=None):
        self.url = url
        self.service_name = service
        self.parent = parent
        self.subs = {}
        if not parent:
            self.client = brpc_client.BRpcClient(self.url, username, passwd)

    def add_sub_service(self, name):
        self.subs[name] = BRpcApi(name, None, None, None, self)

    def remove_sub_service(self, name):
        del self.subs[name]

    def get_sub_service(self, name):
        return self.subs[name]

    def get_client(self):
        if not self.parent:
            return self.client
        return self.parent.get_client()

    def get_service_name(self):
        if self.parent:
            return '%s.%s' % (self.parent.get_service_name(),
                              self.service_name)
        else:
            return self.service_name

    def get_full_ame(self, method):
        return self.get_service_name() + '.' + method

    def call(self, method, args, cast=False):
        ''' invoke method
        @param method: str
            method name to invoke
        @param args: list
            args of method
        '''
        name = self.get_full_ame(method)
        client = self.get_client()
        return client.call(name, args, cast)

    def cast(self, method, args):
        return self.call(method, args, True)

    def ping(self):
        return self.call('ping', [])

    def echo(self, string):
        return self.call('echo', [string])

    def help(self):
        return self.call('help', [])

    def listMethods(self):
        return self.call('listMethods', [])

    def listServices(self):
        return self.call('listServices', [])

    def listVars(self):
        return self.call('listVars', [])

    def signatureOf(self, methodName):
        return self.call('signatureOf', [methodName])

    def typeOfVar(self, varName):
        return self.call('typeOfVar', [varName])
