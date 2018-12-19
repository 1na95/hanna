<%@ page import="java.sql.*" %>
<%@ page import="www.DBConnection" %>
<%@ page import="java.util.regex.*" %>
<% String idx = request.getParameter("idx"); %>
<% String id = (String)session.getAttribute("id"); %>

<html>

<head>
</head>

<body>
<table border="1">
<table><tr><td><jsp:include page="../top.jsp" /></td></tr></table>

<table height="1000" border="1"><tr><td width="100" valign=top><jsp:include page="../left.jsp" /></td><td width="900" valign=top>

<%
Connection conn = DBConnection.getConnection();
ResultSet rs = null;
PreparedStatement pstmt = null;

String sql = "select * from tblBoard where num = ?";
pstmt = conn.prepareStatement(sql);

Pattern evilChars = Pattern.compile("['\"\\-#()@;=*/+]");
idx = evilChars.matcher(idx).replaceAll("");


pstmt.setString(1, idx);
rs = pstmt.executeQuery();
String writer ="";
while(rs.next()) {
int num = rs.getInt(1);
writer = rs.getString(2);
String title = rs.getString(3);
String content = rs.getString(4);
%>

<h1>POST</h1>
<table border="1" width="500">
<tr>
<th>idx</th><td><%=num %></td>
<th>writer</th><td><%=writer%></td>
<th>date</th><td><%=rs.getString(5)%></td>
</tr>

<tr>
<th colspan="2">title</th><td colspan="4"><%=title%></td>
</tr>
<tr>
<th colspan="2">content</th><td colspan="4"><%=content%></td>
</tr>


</td></tr></table><br>
<% } if(id!=null) {%>
<input type="button" value="LIST" onclick="location.href='boardIndex.jsp'">

<% } if(id!=null && id.equals(writer)) {%>
<input type="button" value="DELETE" onclick="location.href='delete.jsp?idx=<%=idx%>'">
<input type="button" value="MODIFY" onclick="location.href='modify.jsp?idx=<%=idx%>'">
<% } %>
</body>
</html>
<% DBConnection.freeConnection(conn, pstmt, rs);%>
