/**
 *
 */
package brpc.client.trans.http;

import java.io.IOException;

/**
 * HttpException
 * @author Administrator
 * @date 2015-9-2
 */
public class HttpException extends RuntimeException {

	/**
	 * serialVersionUID
	 */
	private static final long serialVersionUID = 9002188879715465613L;

	private Exception exception = null;
	private int status = 0;

	public HttpException(IOException exception, String message) {
		super(message);
		this.exception = exception;
	}

	public HttpException(int status, String message) {
		super(message);
		this.status = status;
	}

	public int getStatus() {
		return status;
	}

	@Override
	public String toString() {
		if(this.status > 0) {
			return String.format("HttpException: %s [%d]",
					super.toString(), this.status);
		}
		else {
			return String.format("HttpException: %s [%s]",
					super.toString(), this.exception);
		}
	}
}
