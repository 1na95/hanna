<%@ page import="java.sql.ResultSet" %>
<%@ page import="java.sql.DriverManager" %>
<%@ page import="java.sql.Statement"%>
<%@ page import="java.sql.Connection"%>
<%@ page import="java.sql.SQLException" %>
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
		<td><jsp:include page="top.jsp" /></td>
	</tr>
</table>

<table height="1000" border="1">
	<tr>
		<td width="100" valign=top><jsp:include page="left.jsp" /></td>
		<td width="900" valign=top>
		
		<table border="1">
		<%
Class.forName("oracle.jdbc.driver.OracleDriver");
Connection conn = null;
Statement stmt = null;
ResultSet rs = null;
try {

 String JDBCINFO = "jdbc:oracle:thin:@192.168.111.107:1521:orcl";
 String ORA_USER = "testuser";
 String USER_PASSWD = "1234";
 String SQL_QUERY =
  "select name from test";

 conn = DriverManager.getConnection(JDBCINFO, ORA_USER, USER_PASSWD);
 stmt = conn.createStatement();

 rs = stmt.executeQuery(SQL_QUERY);

  while ( rs.next() ) {
		%>
 <tr><td><%= rs.getString("name") %> </td></tr>
<% }
}catch(SQLException ex) {
%>
 Error : <%= ex.getMessage() %>
 <%
 }finally {
  if (rs != null) try { rs.close();} catch(SQLException ex) {}
   if (stmt != null) try { stmt.close();} catch(SQLException ex) {}
    if (conn != null) try { conn.close(); } catch(SQLException ex) {}
    }
    %>

  </table>

		HELLO<br><br><br><br>HELLO

		</td>

	</tr>
</table>

</body>

</html>
