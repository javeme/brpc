from multiprocessing import Process
import sys
import time

from brpc.client import brpc_api


BRPC_URL = "http://127.0.0.1:8080"  # http:#192.168.1.131


def test_api(num=1000, index=0):
    url = BRPC_URL
    service = "nova"  # http:#192.168.1.131
    username = 'test'
    passwd = '123456'
    api = brpc_api.BRpcApi(url, service, username, passwd)
    print api.ping()
    print api.echo('fake-echo')
    print api.help()

    t1 = time.time()

    for i in range(num):
        try:
            print api.echo('fake-%d' % i)
        except Exception as e:
            print 'Exception: ' + str(e)

    t2 = time.time()
    sec = t2 - t1
    if sec < 1:
        sec = 1
    msg = '[%d] total %d requests in %d seconds, %f requests/s' % (
           index, num, sec, num / sec)
    print msg
    return msg


def start_process(func, *args, **kwargs):
    child_proc = Process(target=func, args=args, kwargs=kwargs)
    child_proc.start()
    return child_proc


def start_n(n=10, count=1000):
    print '====start %d ...' % n
    jobs = []
    for i in range(n):
        process = start_process(test_api, count, i)
        jobs.append(process)
    [process.join() for process in jobs]
    print '====end %d' % n


def main():
    print 'test started'
    start_n(4, count=10000)
    print 'finished'


if __name__ == "__main__":
    # freeze_support()
    sys.exit(main())
