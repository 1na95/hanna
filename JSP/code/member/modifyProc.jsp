<@% page import="www.MemberMgr" %>
<jsp:useBean id="mMgr" class="www.MemberMgr" />
<jsp:useBean id="mBean" class="www.MemberBean" >
	<jsp:setProperty name="mBean" property="*" />
</jsp:useBean>

<%
	boolean result = mMgr.updateMember(mBean);
	String pass = mBean.getPass();

	if(result) {
%>

<script type="text/javascript">
	alert("Modified");
	location.href="../index.jsp";
</script>

<% } else { %>

<script type="text/javascript">
	alert("Failed");
	history.back();
</script>

<% } %>
