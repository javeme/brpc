/**
 *
 */
package brpc.common.types;

/**
 * BrpcMethod
 * @author Administrator
 * @date 2015-8-31
 */
public class BrpcMethod {
	public String method;
	public ObjectList args;
	public String authToken;
	public boolean needReturn;
	public int status;
	public String version;
	public String owner;
	public Object returnValue;

	public String getMethod() {
		return method;
	}
	public void setMethod(String method) {
		this.method = method;
	}
	public ObjectList getArgs() {
		return args;
	}
	public void setArgs(ObjectList args) {
		this.args = args;
	}
	public String getAuthToken() {
		return authToken;
	}
	public void setAuthToken(String authToken) {
		this.authToken = authToken;
	}
	public boolean isNeedReturn() {
		return needReturn;
	}
	public void setNeedReturn(boolean needReturn) {
		this.needReturn = needReturn;
	}
	public int getStatus() {
		return status;
	}
	public void setStatus(int status) {
		this.status = status;
	}
	public String getVersion() {
		return version;
	}
	public void setVersion(String version) {
		this.version = version;
	}
	public String getOwner() {
		return owner;
	}
	public void setOwner(String owner) {
		this.owner = owner;
	}
	public Object getReturnValue() {
		return returnValue;
	}
	public void setReturnValue(Object returnValue) {
		this.returnValue = returnValue;
	}
}
