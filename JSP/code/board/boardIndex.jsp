<%@ page import="java.sql.*" %>
<%@ page import="www.DBConnection"%>
<jsp:useBean id="bMgr" class="www.BoardMgr" />

<%
	String cPath=request.getContextPath();
%>

<html>
<head>
<title>Main Page</title>
</head>
<body>
<table border="1">
<table>
	<tr>
		<td><jsp:include page="../top.jsp" /></td>
	</tr>
</table>

<table height="1000" border="1">
	<tr>
		<td width="100" valign=top><jsp:include page="../left.jsp" /></td>
		<td width="900" valign=top>
		
<h1> POST LIST :
<%
Connection conn = DBConnection.getConnection();
Statement stmt = conn.createStatement();

int total = 0; 

String sql = "select count(*) from tblBoard";
ResultSet rs = stmt.executeQuery(sql);
if(rs.next()) {
	total = rs.getInt(1);
}
out.println(total);

String sqlList = "select num, id, subject, regdate from tblBoard order by num desc";
rs = stmt.executeQuery(sqlList);

%></h1>

<table border="1">
        <tr><th width="73">index</th>
        <th width="73">writer</th>
        <th width="200">title</th>
        <th width="250">date</th>

<% if(total==0) { %>
	<tr align="center">
	<td colspan="4"> There is no registered post. </td></tr>
<% } else {
	while(rs.next()) {
		int idx = rs.getInt(1);
		String id = rs.getString(2);
		String title = rs.getString(3);
		String date = rs.getString(4);
%>
	<tr><td><%=idx %></td>
	<td><%=id %></td>
	<td> <a href='content.jsp?idx=<%=idx %>'><%=title %></a></td>
	<td><%=date %></td></tr>
<% } } %>
</table>
<br><input type="button" value="WRITE" onclick="location.href='write.jsp'">

		</td></tr></table>

</body>
<% DBConnection.freeConnection(conn, stmt, rs);%>
</html>
