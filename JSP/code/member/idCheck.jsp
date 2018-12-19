<%@ page import="java.sql.*, javax.sql.*, java.naming.*" %>
<html>
<head>

<jsp:useBean id="mMgr" class="www.MemberMgr" />
<jsp:setProperty name="mMgr" property="*" />
<%

	String id=(String)request.getParameter("id");
	boolean result = false;
	result = mMgr.checkId(id);
%>
<title>ID Check</title>
</head>
<body bgcolor="#FFFFCC">
	<div align="center">
	<br /> <b><%=id%></b>
	<%
		if (!result) {
			out.println("is already existing ID.<p>");
		}
		else {
			out.println("is available.<p>");
		}
	%>
	<a href="#" onclick="self.close()">Close</a>
</body>
</html>
