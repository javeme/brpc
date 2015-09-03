/**
 *
 */
package brpc.client.trans.http;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.net.HttpURLConnection;
import java.net.InetSocketAddress;
import java.net.ProtocolException;
import java.net.Proxy;
import java.net.URL;
import java.net.URLConnection;
import java.net.URLEncoder;
import java.util.Iterator;
import java.util.Map;


/**
 * HttpConnection
 * @author Administrator
 * @date 2015-9-1
 */
public class HttpConnection {

    private String charset = "utf-8";
	private String contentType = "application/json";

    private Integer connectTimeout = null;
    private Integer socketTimeout = null;
    private String proxyHost = null;
    private Integer proxyPort = null;

    static {
    	URLConnection.setContentHandlerFactory(new HttpContentHandlerFactory());
    }

    /**
     * Do GET request
     * @param url
     * @return HttpResponse
     */
    public HttpResponse doGet(String url, Map<String, String> parameters) {
    	if (!parameters.isEmpty()) {
    		url += "?" + formatParameters(parameters);
    	}

        URLConnection connection = null;
		try {
			connection = openConnection(url);
		} catch (IOException e) {
			throw new HttpException(e, "Can't open connection: " + url);
		}
        HttpURLConnection httpConn = (HttpURLConnection)connection;

        httpConn.setRequestProperty("Accept-Charset", charset);
        httpConn.setRequestProperty("Content-Type", "application/x-www-form-urlencoded");

        try {
			return readResponse(httpConn);
		} catch (IOException e) {
			throw new HttpException(e, "Faild to read response");
		}
    }

	/**
     * Do POST request
     * @param url
     * @param parameterMap
     * @return HttpResponse
     */
    public HttpResponse doPost(String url, String data) {
        URLConnection connection = null;
		try {
			connection = openConnection(url);
		} catch (IOException e) {
			throw new HttpException(e, "Can't open connection: " + url);
		}
        HttpURLConnection httpConn = (HttpURLConnection)connection;

        httpConn.setDoOutput(true);
        try {
			httpConn.setRequestMethod("POST");
		} catch (ProtocolException e) {
			throw new HttpException(e, "Can't set request-method to POST");
		}

        httpConn.setRequestProperty("Accept-Charset", charset);
        httpConn.setRequestProperty("Content-Type", contentType);
        httpConn.setRequestProperty("Content-Length", String.valueOf(data.length()));

        try {
            writeData(httpConn, data);
		} catch (IOException e) {
			throw new HttpException(e, "Faild to send request");
		}

        try {
			return readResponse(httpConn);
		} catch (IOException e) {
			throw new HttpException(e, "Faild to read response");
		}
    }

    /**
     * Open Connection from url
     * @param url
     */
	private URLConnection openConnection(String url) throws IOException {
    	URL localURL = new URL(url);
        URLConnection connection;
        if (proxyHost != null && proxyPort != null) {
            Proxy proxy = new Proxy(Proxy.Type.HTTP,
            		new InetSocketAddress(proxyHost, proxyPort));
            connection = localURL.openConnection(proxy);
        } else {
            connection = localURL.openConnection();
        }
        this.renderRequest(connection);
        return connection;
    }

    /**
     * Render request according setting
     * @param request
     */
    private void renderRequest(URLConnection connection) {
        if (connectTimeout != null) {
            connection.setConnectTimeout(connectTimeout);
        }
        if (socketTimeout != null) {
            connection.setReadTimeout(socketTimeout);
        }
    }

    /**
	 * @summary readData
	 * @author Administrator
	 * @date 2015-9-2
	 * @param httpConn
	 * @return String
     * @throws IOException
	 */
	private static Object readData(HttpURLConnection httpConn) throws IOException {
		Object content = httpConn.getContent();
		if(!(content instanceof InputStream)) {
        	return content;
        }

        ByteArrayOutputStream resultBuffer = new ByteArrayOutputStream();
        InputStream inputStream = null;

        try {
            inputStream = httpConn.getInputStream();

            byte[] buffer = new byte[1024];
            int len = -1;
            while ((len = inputStream.read(buffer)) != -1) {
            	resultBuffer.write(buffer, 0, len);
            }
            resultBuffer.close();
        } finally {
            if (inputStream != null) {
                inputStream.close();
            }
        }
        return resultBuffer.toByteArray();
	}

    /**
	 * @summary writeData
	 * @author Administrator
	 * @date 2015-9-2
	 * @param httpConn
	 * @param data void
     * @throws IOException
	 */
	private static void writeData(HttpURLConnection httpConn, String data) throws IOException {
        OutputStream outputStream = null;
        OutputStreamWriter outputStreamWriter = null;

        try {
            outputStream = httpConn.getOutputStream();
            outputStreamWriter = new OutputStreamWriter(outputStream);

            outputStreamWriter.write(data);
            outputStreamWriter.flush();
        } finally {
            if (outputStreamWriter != null) {
                outputStreamWriter.close();
            }
            if (outputStream != null) {
                outputStream.close();
            }
        }
	}

	/**
	 * @summary readResponse
	 * @author Administrator
	 * @date 2015-9-2
	 * @param httpConn void
	 * @throws IOException
	 */
	private static HttpResponse readResponse(HttpURLConnection httpConn) throws IOException {
        Object result = readData(httpConn);
        int status = httpConn.getResponseCode();
        if(status >= 300) {
        	throw new HttpException(status, result.toString());
        }
        return new HttpResponse(status, result, httpConn.getHeaderFields());
	}

    public static String formatParameters(Map<String, String> parameters) {
        /* Translate parameter map to parameter date string */
        StringBuffer parameterBuffer = new StringBuffer();
        if (parameters != null) {
            Iterator<String> iterator = parameters.keySet().iterator();
            String key = null;
            String value = null;
            while (iterator.hasNext()) {
                key = iterator.next();
                if (parameters.get(key) != null) {
                    value = parameters.get(key);
                } else {
                    value = "";
                }

                parameterBuffer.append(URLEncoder.encode(key));
                parameterBuffer.append("=");
                parameterBuffer.append(URLEncoder.encode(value));
                if (iterator.hasNext()) {
                    parameterBuffer.append("&");
                }
            }
        }
        return parameterBuffer.toString();
    }

    /*
     * Getter & Setter
     */
    public Integer getConnectTimeout() {
        return connectTimeout;
    }

    public void setConnectTimeout(Integer connectTimeout) {
        this.connectTimeout = connectTimeout;
    }

    public Integer getSocketTimeout() {
        return socketTimeout;
    }

    public void setSocketTimeout(Integer socketTimeout) {
        this.socketTimeout = socketTimeout;
    }

    public String getProxyHost() {
        return proxyHost;
    }

    public void setProxyHost(String proxyHost) {
        this.proxyHost = proxyHost;
    }

    public Integer getProxyPort() {
        return proxyPort;
    }

    public void setProxyPort(Integer proxyPort) {
        this.proxyPort = proxyPort;
    }

    public String getCharset() {
        return charset;
    }

    public void setCharset(String charset) {
        this.charset = charset;
    }

	public String getContentType() {
		return contentType;
	}

	public void setContentType(String contentType) {
		this.contentType = contentType;
	}

}
