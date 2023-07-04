<?php
$host = "localhost";
$username = "rennystar";
$password = "rennystar";
$dbname = "CSexp1DB";
$mysqli = new mysqli('localhost', 'username', 'password', 'dbname');

// connect to mysql
if ($mysqli->connect_error) {
  echo $mysqli->connect_error;
  exit();
} else {
  $mysqli->set_charset("utf8");
}

// get the keyword
$keyword=$_POST['keyword'];
if (!isset($keyword) || empty($keyword)) {
    exit();
} elseif (is_numeric($keyword)) {
    $column = 'zip'; // postal code
} elseif (mb_strlen($keyword, 'UTF-8') >= 3) {
    $column = 'addr1'; // address
} else {
    $column = 'kana1'; // address in kana
}

echo "$keyword". "の検索結果";
echo "<br>";

// make the query (considering pagination)
$resultsPerPage = 10; // number of results per page
$page = isset($_GET['page']) ? $_GET['page'] : 1; // current page number

$start = ($page - 1) * $resultsPerPage; // start position of the results
$query = "SELECT *
          FROM zipShizuoka
          WHERE $column LIKE '%$keyword%'
          LIMIT $start, $resultsPerPage";

// Let's GOOOOO
$result = mysqli_query($connection, $query);
if (!$result) {
    die("failed to execute query: " . mysqli_error($connection));
}

echo "<table>";
echo "<tr><th>郵便番号</th><th>住所1</th><th>住所2</th><th>住所3</th></tr>";

while ($row = mysqli_fetch_assoc($result)) {
    echo "<tr>";
    echo "<td>".$row['zip']."</td>";
    echo "<td>".$row['addr1']."</td>";
    echo "<td>".$row['addr2']."</td>";
    echo "<td>".$row['addr3']."</td>";
    echo "</tr>";
}
echo "</table>";

$queryCount = "SELECT COUNT(*) as total FROM zipShizuoka WHERE $column LIKE '%$keyword%'";
$resultCount = mysqli_query($connection, $queryCount);
$rowCount = mysqli_fetch_assoc($resultCount);
$totalResults = $rowCount['total'];
$totalPages = ceil($totalResults / $resultsPerPage);

echo "<div class='pagination'>";
for ($i = 1; $i <= $totalPages; $i++) {
    echo "<a href='search.php?page=$i'>$i</a> ";
}
echo "</div>";

mysqli_close($connection);
?>
