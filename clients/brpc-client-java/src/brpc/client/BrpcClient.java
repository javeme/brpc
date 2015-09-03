/**
 *
 */
package brpc.client;

import java.util.Arrays;
import java.util.List;

import net.sf.json.JSONObject;
import brpc.client.trans.http.HttpRequest;
import brpc.client.trans.http.HttpResponse;
import brpc.common.exceptions.BadResponseException;
import brpc.common.types.BrpcMethod;
import brpc.common.types.ObjectList;
import brpc.common.utils.C;

/**
 * Brpc Client
 * @author Administrator
 * @date 2015-8-31
 */
public class BrpcClient {

	private final String url;
	private String authToken;

	/**
	 * @param url
	 */
	public BrpcClient(String url) {
        this.url = url;
        this.authToken = null;
	}

	/**
	 * @param url
	 * @param username
	 * @param passwd
	 */
	public BrpcClient(String url, String username, String password) {
		this(url);
        this.login(username, password);
	}

	/**
	 * @summary checkResponse
	 * @author Administrator
	 * @date 2015-9-1
	 * @param result void
	 */
	private void checkResponse(BrpcMethod result) {
		List<Integer> status = Arrays.asList(C.BrpcStatus.STATUS_RESPONSE_OK,
				C.BrpcStatus.STATUS_RESPONSE_PENDING);
		if (!status.contains(result.status)) {
			String msg = result.returnValue != null ?
					result.returnValue.toString() :
					"failed to invoke method: " + result.method;
			throw new BadResponseException(result.status, msg);
		}
	}

	/**
	 * @summary doRequest
	 * @author Administrator
	 * @date 2015-9-1
	 * @param result void
	 */
	protected BrpcMethod doRequest(BrpcMethod body) {
		//to json string
		String data = JSONObject.fromObject(body).toString();
		//do request
		HttpRequest req = new HttpRequest(this.url);
		HttpResponse response = req.doPost(data);
		//to json
		JSONObject json = JSONObject.fromObject(response.getContent());
		//json to object
		BrpcMethod result = (BrpcMethod) JSONObject.toBean(json, BrpcMethod.class);
		checkResponse(result);
		return result;
	}

	/**
	 * @summary call
	 * @author Administrator
	 * @date 2015-8-31
	 * @param object
	 * @param method
	 * @param args
	 * @return Object
	 */
	public Object call(String object, String methodName, ObjectList args) {
		BrpcMethod method = new BrpcMethod();
		method.owner = object;
		method.method = methodName;
		method.args = args;
		method.authToken = this.authToken;
		method.needReturn = true;
		method.status = C.BrpcStatus.STATUS_REQUEST;
		method.version = "1.0";

		BrpcMethod result = this.doRequest(method);
		return result.returnValue;
	}

	/**
	 * @summary cast
	 * @author Administrator
	 * @date 2015-8-31
	 * @param object
	 * @param method
	 * @param args
	 * @return Object
	 */
	public Object cast(String object, String methodName, ObjectList args) {
		BrpcMethod method = new BrpcMethod();
		method.owner = object;
		method.method = methodName;
		method.args = args;
		method.authToken = this.authToken;
		method.needReturn = false;
		method.status = C.BrpcStatus.STATUS_REQUEST;
		method.version = "1.0";

		BrpcMethod result = this.doRequest(method);
		return result.returnValue;
	}

	/**
	 * @summary login
	 * @author Administrator
	 * @date 2015-8-31
	 * @param loginArgs void
	 */
	public void login(String username, String password) {
		BrpcMethod method = new BrpcMethod();
		method.method = "login";
		method.args = new ObjectList(username, password);
		method.authToken = "";
		method.needReturn = true;
		method.status = C.BrpcStatus.STATUS_REQUEST_LOGIN;
		method.version = "1.0";
		method.owner = "";

		BrpcMethod result = this.doRequest(method);
		this.authToken = result.authToken;
	}

	/**
	 * @summary logout
	 * @author Administrator
	 * @date 2015-8-31 void
	 */
	public void logout() {
		BrpcMethod method = new BrpcMethod();
		method.method = "logout";
		method.args = null;
		method.authToken = this.authToken;
		method.needReturn = true;
		method.status = C.BrpcStatus.STATUS_REQUEST_LOGOUT;
		method.version = "1.0";
		method.owner = "";

		BrpcMethod result = this.doRequest(method);
		this.authToken = result.authToken;
	}

	/**
	 * @summary hasLogin
	 * @author Administrator
	 * @date 2015-8-31 void
	 */
	public boolean hasLogin() {
        return this.authToken != null;
	}

}
