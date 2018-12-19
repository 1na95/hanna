<%@ page import="java.sql.*"%>
<%@ page import="www.DBConnection"%>
<%
boolean result = false;
Connection conn = null;
PreparedStatement pstmt = null;

conn = DBConnection.getConnection();

String id=(String)session.getAttribute("id");
String title = request.getParameter("title");
String content = request.getParameter("content");

String sql = "insert into tblBoard values(num_seq.nextval,?,?,?,sysdate)";
int updateCount = 0;

pstmt = conn.prepareStatement(sql);

pstmt.setString(1, id);
pstmt.setString(2,title);
pstmt.setString(3,content); 

updateCount = pstmt.executeUpdate();

if(updateCount != 0) result = true;

DBConnection.freeConnection(conn, pstmt);

if(result) {
%>
<script>
	alert("Success");
	location.href="boardIndex.jsp";
</script>
<% } else { %>
<script>
	alert("Failed");
	history.back();
</script>
<% } %>
