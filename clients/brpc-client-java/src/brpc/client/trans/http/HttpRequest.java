/**
 *
 */
package brpc.client.trans.http;

import java.util.Map;


public class HttpRequest {

	private final String url;
	private final HttpConnection conn;

	public HttpRequest(String url) {
		this.url = url;
		this.conn = new HttpConnection();
	}

	public HttpResponse doGet(Map<String, String> parameters) {
		return this.conn.doGet(this.url, parameters);
	}

	public HttpResponse doPost(String data) {
		return this.conn.doPost(this.url, data);
	}
}
