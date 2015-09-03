/**
 *
 */
package brpc.client.trans.http;

import java.util.List;
import java.util.Map;

/**
 * HttpResponse
 * @author Administrator
 * @date 2015-9-1
 */
public class HttpResponse {

	private int status;
	private Object content;
	private final Map<String, List<String>> headers;

	/**
	 * @param status
	 * @param content
	 * @param headers
	 */
	public HttpResponse(int status, Object content,
			Map<String, List<String>> headers) {
		this.setStatus(status);
		this.setContent(content);
		this.headers = headers;
	}

	/**
	 * @param status the status to set
	 */
	public void setStatus(int status) {
		this.status = status;
	}

	/**
	 * @return the status
	 */
	public int getStatus() {
		return status;
	}

	/**
	 * @param content the content to set
	 */
	public void setContent(Object content) {
		this.content = content;
	}

	/**
	 * @return the content
	 */
	public Object getContent() {
		return content;
	}

	/**
	 * @param key
	 * @return the headers of name @key
	 */
	public List<String> getHeaders(String key) {
		return this.headers.get(key);
	}

	/**
	 * @param key
	 * @return the header of name @key
	 */
	public String getHeader(String key) {
		List<String> headers = this.headers.get(key);
		if(headers != null && headers.size() > 0) {
			return headers.get(0);
		}
		return null;
	}
}
