package com.test;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.PrintStream;
import java.io.PrintWriter;
import java.net.Authenticator;
import java.net.HttpURLConnection;
import java.net.PasswordAuthentication;
import java.net.URL;
import java.net.URLConnection;

public class Http {
	public static String sendGetResult(String url, String method, String charset) {
		BufferedReader br = null;
		InputStreamReader isr = null;
		InputStream is = null;
		HttpURLConnection huc = null;

		StringBuffer result = new StringBuffer("");
		URL newUrl = null;
		String line = null;
		try {
			newUrl = new URL(url);
			huc = (HttpURLConnection) newUrl.openConnection();

			huc.setRequestMethod(method);
			is = huc.getInputStream();
			isr = charset != null ? new InputStreamReader(is, charset)
					: new InputStreamReader(is);
			br = new BufferedReader(isr);
			while ((line = br.readLine()) != null) {
				result.append("\r\n");
				result.append(line);
			}
			String str1 = result.toString().trim();
			return str1;
		} catch (Exception e) {
			e.printStackTrace();
			System.out.println("发送" + method + "请求异常！" + e.getMessage());
			return null;
		} finally {
			try {
				if (is != null) {
					is.close();
				}
				if (isr != null) {
					isr.close();
				}
				if (br != null) {
					br.close();
				}
				if (huc != null) {
					huc.disconnect();
				}
				result = null;
			} catch (Exception e) {
				System.out.println("发送" + method + "请求时关闭流和对象出错");
			}
		}
	}

	public static void main(String[] a) {
		System.out
				.println(sendGetResult("http://www.baidu.com", "GET", "UTF-8"));
	}
}