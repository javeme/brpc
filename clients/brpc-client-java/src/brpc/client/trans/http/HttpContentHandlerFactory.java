/**
 *
 */
package brpc.client.trans.http;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.ContentHandler;
import java.net.ContentHandlerFactory;
import java.net.URLConnection;

import brpc.common.utils.Settings;

/**
 * HttpContentHandlerFactory
 * @author Administrator
 * @date 2015-9-3
 */
public class HttpContentHandlerFactory implements ContentHandlerFactory {

    @Override
    public ContentHandler createContentHandler(String mimeType) {
        if (isTextType(mimeType)) {
            return new TextContentHandler();
        } else {
            return null;
        }
    }

	private static boolean isTextType(String mimetype) {
		String[] types = mimetype.split("/");
		if(types.length > 0 && types[0].trim().equals("text")) {
			return true;
		}
		else if(Settings.MIME_TEXT_TYPES.contains(mimetype)) {
			return true;
		}
		return false;
	}

	static class TextContentHandler extends ContentHandler {

	    private static final String DEFAULT_ENCODING = "UTF-8";

        @Override
        public Object getContent(URLConnection connection) throws IOException {
            String encoding = getEncoding(connection.getHeaderField("Content-Type"));
            if (encoding == null) {
                encoding = DEFAULT_ENCODING;
            }

    		StringBuffer resultBuffer = new StringBuffer();
            InputStream inputStream = null;
            InputStreamReader inputStreamReader = null;
            BufferedReader reader = null;
            
            try {
	            inputStream = connection.getInputStream();
	            inputStreamReader = new InputStreamReader(inputStream, encoding);
	            reader = new BufferedReader(inputStreamReader);
	            
	            String temp = null;
	            while ((temp = reader.readLine()) != null) {
	            	resultBuffer.append(temp);
	            }
	        } finally {
	            if (reader != null) {
	                reader.close();
	            }
	            if (inputStreamReader != null) {
	                inputStreamReader.close();
	            }
	            if (inputStream != null) {
	                inputStream.close();
	            }
	        }
            return resultBuffer.toString();
        }

        private String getEncoding(String contentType) {
            String[] headers = contentType.split(";");
            for (String header : headers) {
                String[] params = header.split("=");
                if (params.length == 2) {
                    if (params[0].trim().equals("charset")) {
                        return params[1].trim();
                    }
                }
            }
            return null;
        }
    }
}