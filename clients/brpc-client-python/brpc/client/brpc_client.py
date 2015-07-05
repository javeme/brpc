'''
Created on 2015-6-7

@author: Administrator
'''

import json
import urllib2

from brpc.bluemei.utils import retry


STATUS_REQUEST = 10
STATUS_REQUEST_LOGIN = 11
STATUS_REQUEST_LOGOUT = 12

STATUS_RESPONSE_OK = 20
STATUS_RESPONSE_EVENT = 26
STATUS_RESPONSE_PENDING = 27
STATUS_RESPONSE_EXCEPTION = 28

STATUS_RESPONSE_ERROR = 30
STATUS_RESPONSE_INVALID_METHOD = 31
STATUS_RESPONSE_INVALID_ARGS = 32
STATUS_RESPONSE_DENIED = 33


class BRpcException(Exception):
    def __init__(self, msg):
        self.message = msg

    def __str__(self):
        return str(self.message)

class BadResponseException(BRpcException):
    def __init__(self, response):
        self.message = str(response)

class BRpcClient(object):
    '''brpc client for python'''

    def __init__(self, url, username, passwd, option=None):
        self.url = url
        self.username = username
        self.passwd = passwd
        self.options = option or {}
        self.authToken = None

        self.login(username, passwd)

    @staticmethod
    def _do_http_post(url, data, options):
        json_data = json.dumps(data)
        timeout = options.get('timeout', 3)
        content_type = 'application/json'  # 'text/xml'
        content_encoding = 'utf-8'

        req = urllib2.Request(url)
        req.add_header('Content-type', content_type)
        req.add_header('Content-encoding', content_encoding)
        req.add_data(json_data)

        @retry.retry_count(3, urllib2.socket.timeout)
        def _do_req(req, timeout):
            response = urllib2.urlopen(req, timeout=timeout)
            return response.read()

        # print '>>>>', url
        # print '>>>>', json_data
        response = _do_req(req, timeout)
        # print '<<<<', response
        return json.loads(response)

    def _do_request(self, data, options=None):
        '''send http post'''
        url = self.url
        if options is None:
            options = self.options
        return self._do_http_post(url, data, options)

    def call(self, method, args, cast=False):
        '''invoke method'''
        body = {
            'method': method,
            'args': args,
            'authToken': self.authToken,
            'needReturn': not cast,
            'status': STATUS_REQUEST,
            'version': '1.0',
            'owner': '',
            'returnValue': None
        }
        response = self._do_request(body)
        status = response['status']
        if cast:
            if status != STATUS_RESPONSE_PENDING:
                raise BadResponseException(response)
        else:
            if status != STATUS_RESPONSE_OK:
                raise BadResponseException(response)
        return response['returnValue']


    def login(self, username, passwd):
        '''login'''
        body = {
            'method': 'login',
            'args': [username, passwd],
            'authToken': self.authToken,
            'needReturn': True,
            'status': STATUS_REQUEST_LOGIN,
            'version': '1.0',
            'owner': ''
        }
        response = self._do_request(body)
        if response['status'] != STATUS_RESPONSE_OK:
            raise BadResponseException(response)
        self.authToken = response['authToken']

    def logout(self):
        '''logout'''
        body = {
            'method': 'logout',
            'args': None,
            'authToken': self.authToken,
            'needReturn': True,
            'status': STATUS_REQUEST_LOGOUT,
            'version': '1.0',
            'owner': ''
        }
        response = self._do_request(body)
        if response['status'] != STATUS_RESPONSE_OK:
            raise BadResponseException(response)
        self.authToken = response['authToken']

    def has_login(self):
        '''is login'''
        return self.authToken is not None
