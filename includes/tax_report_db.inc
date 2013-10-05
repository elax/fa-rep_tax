<?php
// ----------------------------------------------------------------
// $ Revision:  1.0 $
// Creator: Alastair Robertson
// date_:   2011-10-22
// Title:   D/b access for Tax inquiry & detail report (cash basis)
// Free software under GNU GPL
// ----------------------------------------------------------------

function db_has_reports()
{
	return check_empty_result("SELECT COUNT(*) FROM ".TB_PREF."tax_rep");
}

function db_has_tax_report_lines($tax_rep_id)
{
	return check_empty_result("SELECT COUNT(*) FROM ".TB_PREF."tax_rep_line WHERE tax_rep_id=".db_escape($tax_rep_id));
}

function db_has_multiple_tax_reports()
{
	$sql = "SELECT COUNT(*) FROM ".TB_PREF."tax_rep WHERE inactive = 0";
	$result = db_query($sql, "could not get tax report count");
	$row = db_fetch_row($result);

	return $row[0];
}

function add_tax_rep($code, $description, $basis,
			$liability_account, $bank_account, $balancing_journal, $tax_payment)
{
	$sql = "INSERT INTO ".TB_PREF."tax_rep (code, description, basis, 
		liability_account, bank_account, balancing_journal, tax_payment) VALUES ("
		.db_escape($code) .", " .db_escape($description) .", "
		.db_escape($basis) . ", " 
		.db_escape($balancing_journal) . ", " 
		.db_escape($liability_account) . ", " 
		.db_escape($tax_payment) . ", " 
		.db_escape($bank_account) . ")";

	db_query($sql,"The tax report could not be added");
}

function update_tax_rep($tax_rep_id, $code, $description, $basis,
			$liability_account, $bank_account, $balancing_journal, $tax_payment)
{
	$sql = "UPDATE ".TB_PREF."tax_rep SET code=" . db_escape($code) . ", 
		description=" . db_escape($description) . ",
		basis=".db_escape($basis) . ", 
		balancing_journal=".db_escape($balancing_journal) . ", 
		liability_account=".db_escape($liability_account) . ", 
		tax_payment=".db_escape($tax_payment) . ", 
		bank_account=".db_escape($bank_account) . " 
		WHERE tax_rep_id = ".db_escape($tax_rep_id);

	db_query($sql,"The tax report could not be updated");
}

function delete_tax_report($tax_rep_id)
{
	begin_transaction();
	$sql = "select * from ".TB_PREF."tax_rep_line_accounts where tax_rep_line_id in (select tax_rep_line_id from ".TB_PREF."tax_rep_line where tax_rep_id = ".db_escape($tax_rep_id).")";
	db_query($sql,"cannot delete tax_rep");
	$sql = "select * from ".TB_PREF."tax_rep_line where tax_rep_id = ".db_escape($tax_rep_id);
	db_query($sql,"cannot delete tax_rep");
	$sql = "DELETE FROM ".TB_PREF."tax_rep WHERE tax_rep_id=".db_escape($tax_rep_id);
	db_query($sql,"cannot delete tax_rep");
	commit_transaction();
}

function get_tax_reports($show_inactive=false)
{
	$sql = "SELECT * FROM ".TB_PREF."tax_rep";
	if (!$show_inactive) $sql .= " WHERE !inactive";
	$sql .= " ORDER BY description";

	return db_query($sql, "could not retrieve tax reports");
}

function get_tax_report($tax_rep_id=0)
{
	$sql = "SELECT tr.*, cm.account_name liability_name FROM ".TB_PREF."tax_rep tr"
		. " LEFT JOIN ".TB_PREF."chart_master cm ON tr.liability_account = cm.account_code ";
	if ($tax_rep_id == 0)
		$sql .= "WHERE inactive=0";
	else
		$sql .= "WHERE tax_rep_id=".db_escape($tax_rep_id);

	$result = db_query($sql, "could not get tax_rep");

	return db_fetch($result);
}

function get_tax_rep_name($tax_rep_id)
{
	$sql = "SELECT code FROM ".TB_PREF."tax_rep WHERE tax_rep_id=".db_escape($tax_rep_id);

	$result = db_query($sql, "could not get tax report");

	$row = db_fetch_row($result);

	return $row[0];
}

function get_tax_rep_by_ref($reference)
{
	$sql = "SELECT * FROM ".TB_PREF."tax_rep WHERE code=".db_escape($reference);

	$result = db_query($sql, "could not get tax report");

	return db_fetch($result);
}

function update_tax_report_line($tax_rep_line_id, $tax_rep_id, $code, $description, $line_type, $calculation, $code_basis, $accounts)
{
	$sql = "UPDATE ".TB_PREF."tax_rep_line SET "
		  ."tax_rep_id=".db_escape($tax_rep_id) . ", "
		  ."code=".db_escape($code) . ", "
		  ."description=".db_escape($description) . ", "
		  ."line_type=".db_escape($line_type) . ", "
		  ."calculation=".db_escape($calculation) . ", "
		  ."code_basis=".db_escape($code_basis) 
		  ." WHERE tax_rep_line_id = ".db_escape($tax_rep_line_id);

	begin_transaction();

	$ret = db_query($sql, "Can't update tax report line");
	if ($ret) {
		$ret = db_query("DELETE FROM ".TB_PREF."tax_rep_line_accounts WHERE tax_rep_line_id = ".db_escape($tax_rep_line_id), "Can't update tax report line accounts");
		if (is_array($accounts)) {
			foreach ($accounts as $acc) {
				$sql = "INSERT INTO ".TB_PREF."tax_rep_line_accounts "
						."(tax_rep_line_id, code) VALUES ("
			  			.db_escape($tax_rep_line_id) . ", "
			  			.db_escape($acc) . ")";
				$ret = db_query($sql, "Can't insert tax report line account");		  			
			}		
		} else {
			$sql = "INSERT INTO ".TB_PREF."tax_rep_line_accounts "
					."(tax_rep_line_id, code) VALUES ("
		  			.db_escape($tax_rep_line_id) . ", "
		  			.db_escape($accounts) . ")";
			$ret = db_query($sql, "Can't insert tax report line account");		  			
		}
	}
	commit_transaction();
}

function add_tax_report_line($tax_rep_id, $code, $description, $line_type, $calculation, $code_basis, $accounts)
{
	begin_transaction();

	$sql = "INSERT INTO ".TB_PREF."tax_rep_line "
			."(tax_rep_id,code,description,line_type,calculation,code_basis, sequence)"
			." SELECT "
		  	.db_escape($tax_rep_id) . ", "
		  	.db_escape($code) . ", "
		  	.db_escape($description) . ", "
		  	.db_escape($line_type) . ", "
		  	.db_escape($calculation) . ", "
		  	.db_escape($code_basis) . ", " 
		  	."COALESCE(MAX(sequence),0) + 1 FROM ".TB_PREF."tax_rep_line WHERE tax_rep_id=".db_escape($tax_rep_id);

	$ret = db_query($sql, "Can't insert tax report line");
	if ($ret) {
		$tax_rep_line_id = mysql_insert_id();
		if (is_array($accounts)) {
			foreach ($accounts as $acc) {
				$sql = "INSERT INTO ".TB_PREF."tax_rep_line_accounts "
						."(tax_rep_line_id, code) VALUES ("
			  			.db_escape($tax_rep_line_id) . ", "
			  			.db_escape($acc) . ")";
				$ret = db_query($sql, "Can't insert tax report line account");		  			
			}		
		} else {
			$sql = "INSERT INTO ".TB_PREF."tax_rep_line_accounts "
					."(tax_rep_line_id, code) VALUES ("
		  			.db_escape($tax_rep_line_id) . ", "
		  			.db_escape($accounts) . ")";
			$ret = db_query($sql, "Can't insert tax report line account");		  			
		}
	}
	commit_transaction();
}

function update_tax_report_line_sequence($tax_rep_line_id, $direction)
{
	begin_transaction();
	$myrow = get_tax_report_line($tax_rep_line_id);
	$sequence  = $myrow["sequence"];
	$tax_rep_id = $myrow["tax_rep_id"];
	if ($direction == 'down') {
		$sql = "select max(sequence) as maxsequence from ".TB_PREF."tax_rep_line where tax_rep_id = ".db_escape($tax_rep_id);
		$res = db_query($sql, "Can't get tax report line");
		$myrow = db_fetch($res);
		$maxsequence  = $myrow["maxsequence"];
		if ($sequence < $maxsequence) {
			$sql = "update ".TB_PREF."tax_rep_line set sequence = sequence - 1 "
				. "where tax_rep_id = ".db_escape($tax_rep_id)
				. " and sequence = ".db_escape($sequence + 1);
			db_query($sql,"cannot update tax report line sequence");
			$sql = "update ".TB_PREF."tax_rep_line set sequence = sequence + 1 "
				. "where tax_rep_line_id = ".db_escape($tax_rep_line_id);
			db_query($sql,"cannot update tax report line sequence up");	
		}
	} else {
		if ($sequence > 1) {
			$sql = "update ".TB_PREF."tax_rep_line set sequence = sequence + 1 "
				. "where tax_rep_id = ".db_escape($tax_rep_id)
				. " and sequence = ".db_escape($sequence - 1);
			db_query($sql,"cannot update tax report line sequence");
			$sql = "update ".TB_PREF."tax_rep_line set sequence = sequence - 1 "
				. "where tax_rep_line_id = ".db_escape($tax_rep_line_id);
			db_query($sql,"cannot update tax report line sequence down");	
		}
	}
	commit_transaction();
}

function delete_tax_report_line($tax_rep_line_id)
{
	begin_transaction();
	$sql = "DELETE FROM ".TB_PREF."tax_rep_line_accounts WHERE tax_rep_line_id = ".db_escape($tax_rep_line_id);
	db_query($sql,"cannot delete tax report line account");
	$sql = "DELETE FROM ".TB_PREF."tax_rep_line WHERE tax_rep_line_id = ".db_escape($tax_rep_line_id);
	db_query($sql,"cannot delete tax report line");	
	commit_transaction();
}

function get_tax_report_lines($tax_rep_id)
{
	$sql = "SELECT trl.*, min(trla.code) as account FROM ".TB_PREF."tax_rep_line trl "
			. "left join 0_tax_rep_line_accounts trla on trla.tax_rep_line_id = trl.tax_rep_line_id "
			."where tax_rep_id=".db_escape($tax_rep_id)
			. " group by tax_rep_line_id"
			." order by sequence";
	return db_query($sql, "Can't get tax report lines");
}

function get_tax_report_line($id)
{
	$sql = "SELECT * FROM ".TB_PREF."tax_rep_line WHERE tax_rep_line_id=".db_escape($id);
	$res = db_query($sql, "Can't get tax report line");
	return db_fetch($res);
}

function get_tax_report_lines_count($tax_rep_id)
{
	$sql = "SELECT COUNT(*) `lines` FROM ".TB_PREF."tax_rep_line WHERE tax_rep_id=".db_escape($tax_rep_id);
	$res = db_query($sql, "Can't get tax report line");
	$row = db_fetch($res);
	return $row['lines'];
}

function get_tax_report_line_accounts($id)
{
	$accs = array();
	$sql = "SELECT * FROM ".TB_PREF."tax_rep_line_accounts WHERE tax_rep_line_id=".db_escape($id);
	$res = db_query($sql, "Can't get tax report line");
	while ($myrow = db_fetch($res)) 
	{
		$accs[] = $myrow['code'];
	}
	return $accs;
}

function get_tax_report_journal($from, $to)
{
    $sql = "SELECT tt.id, tt.name, tt.sales_gl_code, tt.purchasing_gl_code,
    			cm.account_name sales_name, cm2.account_name purchasing_name, 
    			SUM(net_output) net_output,
				SUM(payable) payable,
				SUM(net_input) net_input,
				SUM(collectible) collectible
        FROM (".tax_cash_sql($from, $to, '').") taxrec
        INNER JOIN ".TB_PREF."tax_types tt
        ON taxrec.id = tt.id
        LEFT JOIN ".TB_PREF."chart_master cm ON tt.sales_gl_code = cm.account_code
        LEFT JOIN ".TB_PREF."chart_master cm2 ON tt.purchasing_gl_code = cm2.account_code
        GROUP BY tt.id, tt.name, tt.sales_gl_code, tt.purchasing_gl_code";
	//display_error($sql);
    return db_query($sql,"Cannot retrieve tax journal");
}

function get_tax_accrual_summary($from, $to, $accounts)
{
    $fromdate = date2sql($from);
    $todate = date2sql($to);
    $tax_ids = rtrim(implode(",", $accounts),",");
    
	$sql = "SELECT SUM(net_output + payable) as gross_output,
					SUM(net_output) as net_output, SUM(payable) as payable,
					SUM(net_input + collectible) as gross_input,
					SUM(net_input) as net_input, SUM(collectible) as collectible
			FROM (
				SELECT 
				SUM(IF(trans_type=".ST_CUSTCREDIT." || trans_type=".ST_SUPPINVOICE
					." || trans_type=".ST_JOURNAL.",-1,1)*
				IF(trans_type=".ST_BANKDEPOSIT." || trans_type=".ST_SALESINVOICE 
					." || (trans_type=".ST_JOURNAL ." AND amount<0)"
					." || trans_type=".ST_CUSTCREDIT.", net_amount*ex_rate,0)) net_output,
	
				SUM(IF(trans_type=".ST_CUSTCREDIT." || trans_type=".ST_SUPPINVOICE
					." || trans_type=".ST_JOURNAL.",-1,1)*
				IF(trans_type=".ST_BANKDEPOSIT." || trans_type=".ST_SALESINVOICE
					." || (trans_type=".ST_JOURNAL ." AND amount<0)"
					." || trans_type=".ST_CUSTCREDIT.", amount*ex_rate,0)) payable,
	
				SUM(IF(trans_type=".ST_CUSTCREDIT." || trans_type=".ST_SUPPINVOICE." || trans_type=".ST_JOURNAL.",-1,1)*
				IF(trans_type=".ST_BANKDEPOSIT." || trans_type=".ST_SALESINVOICE
					." || (trans_type=".ST_JOURNAL ." AND amount<0)"
					." || trans_type=".ST_CUSTCREDIT.", 0, net_amount*ex_rate)) net_input,
	
				SUM(IF(trans_type=".ST_CUSTCREDIT." || trans_type=".ST_SUPPINVOICE." || trans_type=".ST_JOURNAL.",-1,1)*
				IF(trans_type=".ST_BANKDEPOSIT." || trans_type=".ST_SALESINVOICE
					." || (trans_type=".ST_JOURNAL ." AND amount<0)"
					." || trans_type=".ST_CUSTCREDIT.", 0, amount*ex_rate)) collectible,
				taxrec.rate,
				ttype.id,
				ttype.name
			FROM ".TB_PREF."tax_types ttype,
				 ".TB_PREF."trans_tax_details taxrec
			WHERE taxrec.tax_type_id=ttype.id
				AND taxrec.trans_type != ".ST_CUSTDELIVERY."
				AND taxrec.tran_date >= '$fromdate'
				AND taxrec.tran_date <= '$todate'
				AND taxrec.tax_type_id IN (".$tax_ids.")
			GROUP BY ttype.id) all_codes";
	$res = db_query($sql,"Cannot retrieve tax summary");
	return db_fetch($res);
}

function get_tax_cash_summary($from, $to, $accounts)
{
    $tax_ids = rtrim(implode(",", $accounts),",");
		
    $sql = "SELECT SUM(gross_output) gross_output,
    			SUM(net_output) net_output,
				SUM(payable) payable,
				SUM(gross_input) gross_input,
				SUM(net_input) net_input,
				SUM(collectible) collectible
        FROM (".tax_cash_sql($from, $to, $tax_ids).") taxrec";
//display_error($sql);
	$res = db_query($sql,"Cannot retrieve tax summary");
	return db_fetch($res);
}

function get_tax_cash_details($from, $to, $accounts)
{
    $tax_ids = rtrim(implode(",", $accounts),",");
	
    $sql = "SELECT *
        FROM (".tax_cash_sql($from, $to, $tax_ids).") taxrec
        ORDER BY name, output desc, trans_date, rate";
//display_error($sql);
    return db_query($sql,"Cannot retrieve tax details");
}

function tax_cash_sql($from, $to, $tax_ids='') {

    $fromdate = date2sql($from);
    $todate = date2sql($to);

    $sql = "SELECT
            'Output',
            (ttd.net_amount+ttd.amount)*ex_rate gross_output,
            ttd.net_amount*ex_rate net_output,
            ttd.amount*ex_rate payable,
            0 gross_input,
            0 net_input,
            0 collectible,
            ttd.rate,
            bt.type,
            bt.person_type_id,
            bt.person_id,
            ttd.trans_no,
            tt.id,
            tt.name,
            bt.trans_date
        FROM ".TB_PREF."bank_trans bt
        INNER JOIN ".TB_PREF."cust_allocations ca
            ON bt.type = ca.trans_type_from
            AND bt.trans_no = ca.trans_no_from
        INNER JOIN ".TB_PREF."debtor_trans dt
            ON dt.type = ca.trans_type_from
            AND dt.trans_no = ca.trans_no_from
        INNER JOIN ".TB_PREF."trans_tax_details ttd
            ON ttd.trans_type = ca.trans_type_to
            AND ttd.trans_no = ca.trans_no_to
        INNER JOIN ".TB_PREF."tax_types tt
            ON tt.id = ttd.tax_type_id
        WHERE bt.trans_date >= '$fromdate'
            AND bt.trans_date <= '$todate'";
    if (strlen(trim($tax_ids)) > 0)
		$sql .= "AND tt.id IN (".$tax_ids.")
		";
	$sql .= "UNION ALL
        SELECT
            'Output',
            bt.amount*ex_rate gross_output,
            ttd.net_amount*ex_rate net_output,
            ttd.amount*ex_rate payable,
            0 gross_input,
            0 net_input,
            0 collectible,
            ttd.rate,
            bt.type,
            bt.person_type_id,
            bt.person_id,
            ttd.trans_no,
            tt.id,
            tt.name,
            bt.trans_date
        FROM ".TB_PREF."bank_trans bt
        INNER JOIN ".TB_PREF."trans_tax_details ttd
            ON ttd.trans_type = bt.type
            AND ttd.trans_no = bt.trans_no
        INNER JOIN ".TB_PREF."tax_types tt
            ON tt.id = ttd.tax_type_id
        WHERE bt.type = ".BT_CREDIT."
            AND bt.person_type_id <> ".PT_SUPPLIER."
            AND bt.trans_date >= '$fromdate'
            AND bt.trans_date <= '$todate'";
    if (strlen(trim($tax_ids)) > 0)
		$sql .= "AND tt.id IN (".$tax_ids.")
		";
	$sql .= "UNION ALL
        SELECT
            'Output',
            bt.amount*ex_rate gross_output,
            ttd.net_amount*ex_rate net_output,
            ttd.amount*ex_rate payable,
            0 gross_input,
            0 net_input,
            0 collectible,
            ttd.rate,
            bt.type,
            bt.person_type_id,
            bt.person_id,
            ttd.trans_no,
            tt.id,
            tt.name,
            bt.trans_date
        FROM ".TB_PREF."bank_trans bt
        INNER JOIN ".TB_PREF."trans_tax_details ttd
            ON ttd.trans_type = bt.type
            AND ttd.trans_no = bt.trans_no
        INNER JOIN ".TB_PREF."tax_types tt
            ON tt.id = ttd.tax_type_id
        WHERE bt.type = ".BT_CHEQUE."
            AND bt.person_type_id = ".PT_CUSTOMER."
            AND bt.trans_date >= '$fromdate'
            AND bt.trans_date <= '$todate'";
    if (strlen(trim($tax_ids)) > 0)
		$sql .= "AND tt.id IN (".$tax_ids.")
		";
	$sql .= "UNION ALL
        SELECT
            'Input',
            0 gross_output,
            0 net_output,
            0 payable,
            bt.amount*ex_rate gross_input,
            ttd.net_amount*bt.amount/round(ov_amount+ov_gst+ov_discount,6)*ex_rate net_input,
            -1*ttd.amount*bt.amount/round(ov_amount+ov_gst+ov_discount,6)*ex_rate collectible,
            ttd.rate,
            bt.type,
            bt.person_type_id,
            bt.person_id,
            ttd.trans_no,
            tt.id,
            tt.name,
            bt.trans_date
        FROM ".TB_PREF."bank_trans bt
        INNER JOIN ".TB_PREF."supp_allocations sa
            ON bt.type = sa.trans_type_from
            AND bt.trans_no = sa.trans_no_from
        INNER JOIN ".TB_PREF."supp_trans st
            ON st.type = sa.trans_type_from
            AND st.trans_no = sa.trans_no_from
        INNER JOIN ".TB_PREF."trans_tax_details ttd
            ON ttd.trans_type = sa.trans_type_to
            AND ttd.trans_no = sa.trans_no_to
        INNER JOIN ".TB_PREF."tax_types tt
            ON tt.id = ttd.tax_type_id
        WHERE bt.trans_date >= '$fromdate'
            AND bt.trans_date <= '$todate'";
    if (strlen(trim($tax_ids)) > 0)
		$sql .= "AND tt.id IN (".$tax_ids.")
		";
	$sql .= "UNION ALL
        SELECT
            'Input',
            0 gross_output,
            0 net_output,
            0 payable,
            bt.amount*ex_rate gross_input,
            ttd.net_amount*ex_rate net_input,
            ttd.amount*ex_rate collectible,
            ttd.rate,
            bt.type,
            bt.person_type_id,
            bt.person_id,
            ttd.trans_no,
            tt.id,
            tt.name,
            bt.trans_date
        FROM ".TB_PREF."bank_trans bt
        INNER JOIN ".TB_PREF."trans_tax_details ttd
            ON ttd.trans_type = bt.type
            AND ttd.trans_no = bt.trans_no
        INNER JOIN ".TB_PREF."tax_types tt
            ON tt.id = ttd.tax_type_id
        WHERE bt.type <> ".BT_CREDIT."
            AND bt.person_type_id <> ".PT_CUSTOMER."
            AND bt.trans_date >= '$fromdate'
            AND bt.trans_date <= '$todate'";
    if (strlen(trim($tax_ids)) > 0)
		$sql .= "AND tt.id IN (".$tax_ids.")
		";
	$sql .= "UNION ALL
        SELECT
            'Input',
            0 gross_output,
            0 net_output,
            0 payable,
            bt.amount*ex_rate gross_input,
            ttd.net_amount*ex_rate net_input,
            ttd.amount*ex_rate collectible,
            ttd.rate,
            bt.type,
            bt.person_type_id,
            bt.person_id,
            ttd.trans_no,
            tt.id,
            tt.name,
            bt.trans_date
        FROM ".TB_PREF."bank_trans bt
        INNER JOIN ".TB_PREF."trans_tax_details ttd
            ON ttd.trans_type = bt.type
            AND ttd.trans_no = bt.trans_no
        INNER JOIN ".TB_PREF."tax_types tt
            ON tt.id = ttd.tax_type_id
        WHERE bt.type = ".BT_CHEQUE."
            AND bt.person_type_id = ".PT_SUPPLIER."
            AND bt.trans_date >= '$fromdate'
            AND bt.trans_date <= '$todate'";
    if (strlen(trim($tax_ids)) > 0)
		$sql .= "AND tt.id IN (".$tax_ids.")";
  return $sql;
}

function get_tax_account_balance($from_date, $to_date, $accounts)
{
	$from = date2sql($from_date);
	$to = date2sql($to_date);
	$account_list = rtrim(implode(",",$accounts),",");

    $sql = "SELECT SUM(amount) FROM ".TB_PREF."gl_trans
		WHERE tran_date > '$from'
		AND tran_date < '$to'";
    if (strlen(trim($account_list)) > 0)
		$sql .= "AND account IN ('$account_list')";
    
	$result = db_query($sql, "The starting balance for accounts $account_list could not be calculated");

	$row = db_fetch_row($result);
	return $row[0];
}

?>
