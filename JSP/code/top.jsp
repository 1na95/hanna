<%
	String id = (String)session.getAttribute("id");
	String cPath = request.getContextPath();
	
	String memberUrl = "http://192.168.111.108:8080/tomcatdemo/member/join.jsp";
	String memberLabel = "JOIN";
	String logUrl = "http://192.168.111.108:8080/tomcatdemo/member/login.jsp";
	String logLabel = "LOGIN";

	if(id!=null) {
		memberUrl = "http://192.168.111.108:8080/tomcatdemo/member/modify.jsp";
		memberLabel = "MODIFY";
		logUrl = "http://192.168.111.108:8080/tomcatdemo/member/logout.jsp";
		logLabel = "LOGOUT";
	}
%>
<html>
<head>
<title>tomcat-homepage</title>
</head>
<body>
<table width="1000" height="100" border="1" cellpadding="0" cellspacing="0" aling="centet">
	<tr align="center">
		<td width="25%"><a href="http://192.168.111.108:8080/tomcatdemo/index.jsp" target="_parent">HOME</a></td>
		<td width="25%"><a href="http://192.168.111.108:8080/tomcatdemo/board/boardIndex.jsp">BOARD</a></td>
		<td width="25%"><a href="<%=memberUrl%>"><b><%=memberLabel%></b></a></td>
		
		<td width="25%"><a href="<%=logUrl%>"><b><%=logLabel%></b></a></td>
</body>
</html>
