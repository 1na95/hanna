<%
	if(session!=null) session.invalidate();
%>
<script type="text/javascript">
alert("Logout is complete.");
location.href="../index.jsp";
</script>
