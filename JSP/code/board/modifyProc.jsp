<%@ page import="java.sql.*" %>
<%@ page import="www.DBConnection" %>
<% String idx = request.getParameter("idx");
String subject = request.getParameter("title");
String content = request.getParameter("content");

Connection conn = DBConnection.getConnection();
ResultSet rs = null;
PreparedStatement pstmt = null;
int update = 0;
boolean result = false;
String sql = "update tblBoard set subject=?, content=? where num=?";

pstmt = conn.prepareStatement(sql);
pstmt.setString(1, subject);
pstmt.setString(2, content);
pstmt.setString(3, idx);
update = pstmt.executeUpdate();

if(update!=0) result = true;
if(result) {
%>
<script>
	alert("success");
	location.href="content.jsp?idx=<%=idx%>";
</script>
<% } else { %>
<script>
	alert("failed");
	history.back();
</script>
<% } 
DBConnection.freeConnection(conn, pstmt, rs);
%>
