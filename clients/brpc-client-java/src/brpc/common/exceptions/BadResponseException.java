/**
 *
 */
package brpc.common.exceptions;

/**
 * BadResponseException
 * @author Administrator
 * @date 2015-9-1
 */
public class BadResponseException extends BrpcException {

	/**
	 * serialVersionUID
	 */
	private static final long serialVersionUID = -6420096400468188339L;

	private final int status;

	/**
	 * @param status
	 * @param returnValue
	 */
	public BadResponseException(int status, String message) {
		super(message);
		this.status = status;
	}

	public int getStatus() {
		return status;
	}

	@Override
	public String toString() {
		return String.format("BadResponseException: %s [%d]",
				super.toString(), this.status);
	}
}
