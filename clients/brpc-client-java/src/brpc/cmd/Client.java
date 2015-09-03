/**
 *
 */
package brpc.cmd;

import brpc.client.BrpcApi;
import brpc.common.types.ObjectList;
import brpc.common.utils.Settings;

/**
 * entry
 * @author Administrator
 * @date 2015-8-31
 */
public class Client {

	private final BrpcApi api;

	public Client() {
		this.api = new BrpcApi(Settings.SERVICE, Settings.URL,
				Settings.USERNAME, Settings.PASSWORD);
	}

	public Object execute(String[] argv) {
		int argc = argv.length;
		if(argc > 0) {
			String method = argv[0];
			ObjectList args = new ObjectList();
			for(int i=1; i<argc; i++){
				String arg = argv[i];
				char t = arg.charAt(0);
				if (t == '"')
					args.addValue(arg.substring(1, arg.length()-1));
				else if (t == '\'')
					args.addValue(arg.substring(1, arg.length()-1));
				else if (Character.isDigit(t)){
					if (arg.indexOf(".") > 0)
						args.addValue(Float.valueOf(arg));
					else
						args.addValue(Integer.valueOf(arg));
				}
				else if (t == 't' && arg == "true")
					args.addValue(true);
				else if (t == 'f' && arg == "false")
					args.addValue(false);
				else {
					printf(String.format("invalid para: %s\n", argv[i]));
					return null;
				}
			}

			Object result = api.call(method, args);

			String rs = String.format(">>>>> call method %s(%s): \n" +
				"=====\n" +
				"%s\n" +
				"=====\n",
				method,
				args,
				result != null ? result.toString() : "<null>");
			printf(rs);

			return result;
		}
		else {
			String usage = "usage: brpc METHOD [parameters...]\n";
			printf(usage);
			return null;
		}
	}

	public void test() {
		String method = "help";
		ObjectList params = new ObjectList();
		Object result = api.call(method, params);
		System.out.println(result);

		method = "execute";
		params = new ObjectList("dir");
		result = api.call(method, params);
		System.out.println(result);

		method = "execute";
		params = new ObjectList("ping baidu.com");
		result = api.call(method, params);
		System.out.println(result);

		method = "execute";
		params = new ObjectList("notepad abc.txt");
		result = api.cast(method, params);
		System.out.println(result);
	}

	public static void printf(Object obj) {
		System.out.print(obj);
	}

	/**
	 * @summary main
	 * @author Administrator
	 * @date 2015-8-31
	 * @param args void
	 */
	public static void main(String[] args) {
		Client client = new Client();
		client.execute(args);

		//client.test();
	}

}
