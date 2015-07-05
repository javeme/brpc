'''
Created on 2015-6-7

@author: Administrator
'''
import time
import unittest

from brpc.client import brpc_api


BRPC_URL = "http://192.168.1.135:8080"  # http:#192.168.1.131


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


def test_api():
    url = BRPC_URL
    service = "nova"  # http:#192.168.1.131
    username = 'test'
    passwd = '123456'
    api = brpc_api.BRpcApi(url, service, username, passwd)
    print api.ping()
    print api.echo('fake-echo')
    print api.help()

    t1 = time.time()
    num = 1000

    for i in range(num):
        try:
            print api.echo('fake-%d' % i)
        except Exception as e:
            print 'Exception: ' + str(e)

    t2 = time.time()
    sec = t2 - t1
    if sec < 1:
        sec = 1
    msg = 'total %d requests in %d seconds, %f requests/s' % (num, sec,
                                                              num / sec)
    print msg


if __name__ == "__main__":
    # import sys;sys.argv = ['', 'Test.testName']
    print 'test started'
    # test_api()
    unittest.main()
    print 'finished'
