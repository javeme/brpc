/**
 *
 */
package brpc.common.utils;

/**
 * ¿‡√Ë ˆ
 * @author Administrator
 * @date 2015-9-1
 */
public final class C {

	public static final class BrpcStatus {

		public static final int STATUS_REQUEST = 10;
		public static final int STATUS_REQUEST_LOGIN = 11;
		public static final int STATUS_REQUEST_LOGOUT = 12;

		public static final int STATUS_RESPONSE_OK = 20;
		public static final int STATUS_RESPONSE_EVENT = 26;
		public static final int STATUS_RESPONSE_PENDING = 27;
		public static final int STATUS_RESPONSE_EXCEPTION = 28;

		public static final int STATUS_RESPONSE_ERROR = 30;
		public static final int STATUS_RESPONSE_INVALID_METHOD = 31;
		public static final int STATUS_RESPONSE_INVALID_ARGS = 32;
		public static final int STATUS_RESPONSE_DENIED = 33;
	}
}
