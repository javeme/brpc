'''
Created on 2015-6-7

@author: Administrator
'''
import unittest

from brpc.client import brpc_api


BRPC_URL = "http://127.0.0.1:8080"  # http:#192.168.1.131


class TestBRpcApi(unittest.TestCase):
    '''Test for BRpcApi'''
    def setUp(self):
        url = BRPC_URL
        service = "nova"  # http:#192.168.1.131
        username = 'test'
        passwd = '123456'
        self.api = brpc_api.BRpcApi(url, service, username, passwd)

    def test_echo(self):
        s = 'fake-123'
        actual = self.api.echo(s)
        expected = 'MyRpcService: ' + s
        self.assertEqual(expected, actual, 'echo %s?=%s' % (expected, actual))

    def test_ping(self):
        expected = 'BRPC nova/1.0'
        actual = self.api.ping()
        self.assertEqual(expected, actual, 'ping %s?=%s' % (expected, actual))


def main():
    print 'test started'
    # test_api()
    unittest.main(exit=False)
    print 'finished'


if __name__ == "__main__":
    # import sys;sys.argv = ['', 'Test.testName']
    main()
