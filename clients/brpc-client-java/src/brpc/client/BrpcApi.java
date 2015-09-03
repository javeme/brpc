/**
 *
 */
package brpc.client;

import java.util.HashMap;
import java.util.List;

import brpc.common.exceptions.BrpcException;
import brpc.common.exceptions.InvalidArgsException;
import brpc.common.types.ObjectList;

/**
 * Brpc Api
 * @author Administrator
 * @date 2015-8-31
 */
public class BrpcApi {

	private String serviceName;
	private BrpcClient rpcClient;
	private BrpcApi parent;
	private HashMap<String, BrpcApi> subServices;

	private static final ObjectList nullArgs = new ObjectList();

	public BrpcApi(String service, BrpcApi parent)
	{
		this.init(service, parent, null);
	}

	public BrpcApi(String service, String url, String username, String passwd)
	{
		BrpcClient rpcClient = new BrpcClient(url, username, passwd);
		this.init(service, null, rpcClient);
	}

	protected void init(String service, BrpcApi parent, BrpcClient rpcClient)
	{
		this.serviceName = service;
		this.rpcClient = rpcClient;
		this.parent = parent;

		if (this.parent != null) {
			this.rpcClient = this.parent.rpcClient;
			if(!this.parent.addSubSevice(this)) {
				throw new BrpcException("add service failed");
			}
		}
		else if (this.rpcClient == null) {
			throw new InvalidArgsException("both parent and rpcClient are null");
		}
	}

	public void logout()
	{
		this.rpcClient.logout();
	}

	public String ping()
	{
		return this.call("ping", nullArgs);
	}

	public String echo(String str)
	{
		ObjectList args = new ObjectList();
		args.addValue(str);
		return this.call("echo", args);
	}

	public String help()
	{
		return this.call("help", nullArgs);
	}

	public List<String> listMethods()
	{
		return this.call("listMethods", nullArgs);
	}

	public List<String> listVars()
	{
		return this.call("listVars", nullArgs);
	}

	public List<String> listServices()
	{
		return this.call("listServices", nullArgs);
	}

	public List<String> listEextendServices()
	{
		return this.call("listEextendServices", nullArgs);
	}

	public String signatureOf(String method)
	{
		ObjectList args = new ObjectList();
		args.addValue(method);
		return this.call("signatureOf", args);
	}

	public String typeOfVar(String var)
	{
		ObjectList args = new ObjectList();
		args.addValue(var);
		return this.call("typeOfVar", args);
	}

	public boolean subscribe(String event, String method)
	{
		ObjectList args = new ObjectList();
		args.addValue(event);
		args.addValue(method);
		return this.call("subscribe", args);
	}

	public boolean unsubscribe(String event, String method)
	{
		ObjectList args = new ObjectList();
		args.addValue(event);
		args.addValue(method);
		return call("unsubscribe", args);
	}

	public boolean addSubSevice(BrpcApi service)
	{
		return null != subServices.put(service.name(), service);
	}

	public BrpcApi removeSubSevice(String name)
	{
		BrpcApi service = this.subServices.remove(name);
		return service;
	}

	public BrpcApi getSubService(String name)
	{
		return subServices.get(name);
	}

	public String name()
	{
		return this.serviceName;
	}

	public String object()
	{
		if (parent != null)
			return String.format("%s.%s", parent.object(), name());
		else
			return this.name();
	}

	public <T> T call(String method, ObjectList args) {
		return this.call(object(), method, args);
	}

	public <T> T call(String object, String method, ObjectList args) {
		Object result = this.rpcClient.call(object, method, args);
		return (T)result;
	}

	public <T> T cast(String method, ObjectList args) {
		return this.cast(object(), method, args);
	}

	public <T> T cast(String object, String method, ObjectList args) {
		Object result = this.rpcClient.cast(object, method, args);
		return (T)result;
	}

	/*
	public Object call(String method, ObjectList args) {
		return this.call(object(), method, args);
	}

	public Object call(String object, String method, ObjectList args) {
		Object result = this.rpcClient.call(object, method, args);
		return result;
	}

	public Object cast(String method, ObjectList args) {
		return this.cast(object(), method, args);
	}

	public Object cast(String object, String method, ObjectList args) {
		Object result = this.rpcClient.cast(object, method, args);
		return result;
	}*/

}
