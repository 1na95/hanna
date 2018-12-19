<html>
<head>
<title>Main Page</title>

<script type="text/javascript">
function formCheck() {
        var title=document.forms[0].title.value;
        if(title==null) {
                alert("please input title");
                document.forms[0].title.focus();
                return fales;
        }
	if(content==null || contest=="") {
		alert("please input content");
		document.forms[0].content.focus();
		return false;
	}
}
</script>

</head>

<body>
<table border="1">
<table>
        <tr>
                <td><jsp:include page="../top.jsp" /></td>
        </tr>
</table>
<table height="1000" border="1">
        <tr><td width="100" valign=top><jsp:include page="../left.jsp" /></td>
	<td width="900" valign=top>
	<br/>

	<table border="1"><tr><td>
	<form action="writeProc.jsp" action="post"><br>
	subject <input type="text" name="title" size="50"><br/><br />
	content <textarea rows="10" cols="50" name="content"></textarea><br/><br>
	<input type="submit"/>
	</form>
	</tr></td></table>

	</td></tr>
</table>

</body>

</html>

