/**
 *
 */
package brpc.common.types;

import java.util.ArrayList;


/**
 * ObjectList
 * @author Administrator
 * @date 2015-8-31
 */
public class ObjectList extends ArrayList<Object> {

	private static final long serialVersionUID = 614211304267680203L;

	/**
	 * @param username
	 * @param password
	 */
	public ObjectList(Object...args) {
		for (Object arg: args) {
			this.add(arg);
		}
	}

	/**
	 * @summary addValue
	 * @author Administrator
	 * @date 2015-8-31
	 * @param method void
	 */
	public <T> boolean addValue(T arg) {
		return this.add(arg);
	}

}
