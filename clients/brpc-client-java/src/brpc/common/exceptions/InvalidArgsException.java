/**
 *
 */
package brpc.common.exceptions;

/**
 * InvalidArgsException
 * @author Administrator
 * @date 2015-8-31
 */
public class InvalidArgsException extends RuntimeException {

	/**
	 * serialVersionUID
	 */
	private static final long serialVersionUID = 1538202832562499817L;

	/**
	 * @param string
	 */
	public InvalidArgsException(String msg) {
		super(msg);
	}

}
