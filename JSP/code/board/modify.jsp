<%@ page import="java.sql.*" %>
<%@ page import="www.DBConnection" %>
<html>
<head>
<title>Main Page</title>
</head>

<body>
<table border="1">
<table><tr><td><jsp:include page="../top.jsp" /></td></tr></table>

<table height="1000" border="1"><tr><td width="100" valign=top><jsp:include page="../left.jsp" /></td><td width="900" valign=top><table border="1">

<%
String idx = request.getParameter("idx");
Connection conn = DBConnection.getConnection();
PreparedStatement pstmt = null;
ResultSet rs = null;

String sql = "select * from tblBoard where num=?";
pstmt = conn.prepareStatement(sql);
pstmt.setString(1, idx);
rs = pstmt.executeQuery();

String id = "";
String title = "";
String content = "";
String regDate = "";

while(rs.next()) {
id = rs.getString(2);
title = rs.getString(3);
content = rs.getString(4);
regDate = rs.getString(5);

%>
<h1>POST</h1>
        <table border="1">
	<tr><td>
	<table>
        <form action="modifyProc.jsp" method="post">
		<tr><td align="center"><b> idx</b> <td><input name="idx" type="text" readonly value="<%=idx%>" size="5"></td></td>
		<td align="center"><b>writer</b> <td><input name="id" type="text" readonly value="<%=id%>" size="5"></td></td>
		<td align="center"><b> date</b><td><input name="regDate" type="text" readonly value="<%=regDate%>"</td></td></tr>
		<tr><td align="center"><b>title</b><td colspan="5"><input name="title" type="text" value="<%=title%>" size="50"></td></td></tr>
		<tr><td align="center"><b>content</b><td colspan="5"> <textarea name="content" rows="10" cols="50"><%=content%></textarea></td></td></tr>
		<tr><td colspan="6"align="center"><input type="submit" value="modify"></td></tr>
	</table></form>
	</tr></td></table>

</tr></td></table>
<% }
DBConnection.freeConnection(conn, pstmt, rs);
%>
</body>
</html>
