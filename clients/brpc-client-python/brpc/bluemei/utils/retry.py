'''
Created on 2015-7-5

@author: Administrator
'''

import time


def retry_count(count, exceptions, interval=0):
    def _retry(method, *args, **kwargs):
        ex = None
        for i in range(count):
            try:
                # print 'retry:', i
                return method(*args, **kwargs)
            except exceptions as e:
                ex = e
                time.sleep(interval)
        if ex:
            raise ex
        else:
            raise Exception('Failed after retry count %d' % i)

    def _fun(f):
        def _fun2(*args, **kwargs):
            return _retry(f, *args, **kwargs)
        return _fun2
    return _fun
