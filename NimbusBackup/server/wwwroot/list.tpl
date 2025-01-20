<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 3.2 Final//EN">
<html>
  <head>
    <meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
    <title>Download</title>
  </head>
  <body>
    <h1>Download</h1>
    <table>
      <tr>
        <th valign="top"><img src="icons/gif/16x16/List.gif"></th>
        <th>Name</th>
        <th>Last modified</th>
        <th>Size</th>
        <th>Description</th>
      </tr>
      <tr><th colspan="5"><hr></th></tr>
      
      <!-- 父目录行（如果需要，可单独写死或者也做成Section  -->
      <tr>
        <td valign="top"><img src="icons/gif/16x16/Back.gif"></td>
        <td><a href="/">Parent Directory</a></td>
        <td>&nbsp;</td>
        <td align="right">-</td>
        <td>&nbsp;</td>
      </tr>

      <!-- 这里是一个循环区域，ctemplate使用 Section 来表示循环 -->
      {{#FILE_LIST}}
      <tr>
        <td valign="top"><img src="{{ICON_PATH}}"></td>
        <td><a href="{{DOWNLOAD_LINK}}">{{FILE_NAME}}</a></td>
        <td align="right">{{LAST_MODIFIED}}</td>
        <td align="right">{{FILE_SIZE}}</td>
        <td>&nbsp;</td>
      </tr>
      {{/FILE_LIST}}

      <tr><th colspan="5"><hr></th></tr>
    </table>
    <address>Server build by ffengc (https://github.com/ffengc) Port 8080</address>
  </body>
</html>