/**
 *
 */
package tests.brpc.client;

import junit.framework.TestCase;
import brpc.client.BrpcApi;
import brpc.common.utils.Settings;

/**
 * Test for class brpc.client.BrpcApi
 * @author Administrator
 * @date 2015-8-31
 */
public class TestBrpcApi extends TestCase {

	private BrpcApi api;

	/* (non-Javadoc)
	 * @see junit.framework.TestCase#setUp()
	 */
	@Override
	protected void setUp() throws Exception {
		super.setUp();
		this.api = new BrpcApi(Settings.SERVICE, Settings.URL,
				Settings.USERNAME, Settings.PASSWORD);
	}

	/* (non-Javadoc)
	 * @see junit.framework.TestCase#tearDown()
	 */
	@Override
	protected void tearDown() throws Exception {
		super.tearDown();
		this.api.logout();
	}

	protected void assertEqual(String method, String expected, String actual) {
    	String msg = String.format("%s %s?=%s", method, expected, actual);
		assertEquals(msg, expected, actual);
	}

    public void testEcho() {
    	String s = "fake-123";
    	String actual = this.api.echo(s);
    	String expected = "MyRpcService: " + s;
        this.assertEqual("echo", expected, actual);
    }

    public void testPing() {
        String expected = "BRPC nova/1.0";
        String actual = this.api.ping();
        this.assertEqual("ping", expected, actual);
    }
}
