/**
 *
 */
package brpc.common.utils;

import java.util.Arrays;
import java.util.HashMap;
import java.util.List;


/**
 * Settings
 * @author Administrator
 * @date 2015-8-31
 */
public class Settings {

	public static SettingsLoader settings = new DefaultSettingsLoader();

	public static final String URL = getString("url");
	public static final String USERNAME = getString("username");
	public static final String PASSWORD = getString("password");
	public static final String SERVICE = getString("service");

	public static final List<String> MIME_TEXT_TYPES = getStrings("mime_type_text");

	public static String getString(String name) {
		return (String) settings.get(name);
	}

	public static List<String> getStrings(String name) {
		return (List<String>) settings.get(name);
	}

	public static int getInt(String name) {
		return Integer.valueOf(getString(name));
	}
}


/**
 * SettingsLoader
 * @author Administrator
 * @date 2015-8-31
 */
interface SettingsLoader {
	public Object get(String name);
}


/**
 * DefaultSettingsLoader
 * @author Administrator
 * @date 2015-8-31
 */
class DefaultSettingsLoader implements SettingsLoader {

	private HashMap<String, Object> settings = null;

	public DefaultSettingsLoader() {
		this.settings = new HashMap<String, Object>();
		this.settings.put("url", "http://127.0.0.1:8080");
		this.settings.put("username", "test");
		this.settings.put("password", "123456");
		this.settings.put("service", "nova");

		List<String> mimeTextTypes = Arrays.asList(
			"text/plain",
		    "text/html",
		    "text/css",
		    "text/csv",
		    "application/javascript",
		    "application/json",
		    "application/xml");
		this.settings.put("mime_type_text", mimeTextTypes);
	}

	/* (non-Javadoc)
	 * @see brpc.common.utils.SettingsLoader#get(java.lang.String)
	 */
	@Override
	public Object get(String name) {
		return this.settings.get(name);
	}
}
