/**
 *
 */
package brpc.common.exceptions;

/**
 * BrpcException
 * @author Administrator
 * @date 2015-8-31
 */
public class BrpcException extends RuntimeException {

	/**
	 * serialVersionUID
	 */
	private static final long serialVersionUID = -313629876571002111L;

	/**
	 * @param string
	 */
	public BrpcException(String msg) {
		super(msg);
	}

}
