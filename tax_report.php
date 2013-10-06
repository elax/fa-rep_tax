<?php
// ----------------------------------------------------------------
// $ Revision:  1.0 $
// Creator: Alastair Robertson
// date_:   2011-10-22
// Title:   Tax inquiry (cash basis)
// Free software under GNU GPL
// ----------------------------------------------------------------
$page_security = 'SA_TAXREPEX';
$path_to_root="../..";
include_once($path_to_root . "/includes/session.inc");
add_access_extensions();

include_once($path_to_root . "/includes/date_functions.inc");
include_once($path_to_root . "/includes/ui.inc");
include_once($path_to_root . "/includes/data_checks.inc");

include($path_to_root . "/modules/rep_tax/includes/tax_report_db.inc");
include_once($path_to_root . "/modules/rep_tax/includes/tax_report_ui.inc");
include_once($path_to_root . "/modules/rep_tax/includes/evalmath.class.php");

$js = '';
set_focus('account');
if ($use_popup_windows)
	$js .= get_js_open_window(800, 500);
if ($use_date_picker)
	$js .= get_js_date_picker();

page(_($help_context = "Tax Report"), false, false, '', $js);

//----------------------------------------------------------------------------------------------------
// Ajax updates
//
if (get_post('Show')) 
{
	$Ajax->activate('trans_tbl');
}

if (isset($_GET['tax_rep_id']))
	$_POST['tax_rep_id'] = $_GET['tax_rep_id'];
	
$company_prefs = get_company_prefs();

if (get_post('TransFromDate') == "" && get_post('TransToDate') == "")
{
	$date = Today();
	$edate = add_months($date, -$company_prefs['tax_last']);
	$edate = end_month($edate);
	$bdate = begin_month($edate);
	$bdate = add_months($bdate, -$company_prefs['tax_prd'] + 1);
	$_POST["TransFromDate"] = $bdate;
	$_POST["TransToDate"] = $edate;
}	

//----------------------------------------------------------------------------------------------------

function tax_inquiry_controls()
{	
	start_form();

    start_table(TABLESTYLE_NOBORDER);

    $taxReports = db_has_multiple_tax_reports();
    if ($taxReports > 0) {
	    if ($taxReports > 1) {
	    	tax_report_list_cells(_('Tax Report').":",'tax_rep_id');
	   	} else {
	   		$tax_report = get_tax_report();
	   		$_POST['tax_rep_id'] = $tax_report['tax_rep_id'];
	   		label_cell('<b>'.$tax_report['description'].'</b>','colspan=5 align=center');
	   	}
	    start_row();
		date_cells(_("from:"), 'TransFromDate', '', null, -30);
		date_cells(_("to:"), 'TransToDate');
		submit_cells('Show',_("Show"),'','', 'default');
	    end_row();
    } else {
    	display_notification_centered(_('At least one tax report has to be activated.'));	
    }
    
	end_table();

    end_form();
}

//----------------------------------------------------------------------------------------------------

function calculate_tax_code($basis, $from, $to, $code_basis, $accounts)
{
	if ($basis == 'Accrual') {
		$result = get_tax_accrual_summary($from, $to, $accounts);
	} else {
		$result = get_tax_cash_summary($from, $to, $accounts);
	}
	$value = 0;
	switch ($code_basis) {
		case 'SalesGross':
			$value = $result['gross_output'];
			break;
		case 'SalesNet':
			$value = $result['net_output'];
			break;
		case 'SalesTax':
			$value = $result['payable'];
			break;
		case 'PurchaseGross':
			$value = 0 - $result['gross_input'];
			break;
		case 'PurchaseNet':
			$value = 0 - $result['net_input'];
			break;
		case 'PurchaseTax':
			$value = 0 - $result['collectible'];
			break;
	}
	//return empty($value) ? 0 : $value;
	return $value;
}

function calculate_computation($formula, $calculated)
{
	$calc_array = explode(" ", $formula);
	foreach($calc_array as &$item)
	{
		$search_item = trim($item);
		// strip leading { and trailing } to search for numeric code
		if (preg_match("/^\{[0-9a-zA-Z]+\}$/", $search_item)) {
			$search_item = ltrim(rtrim($search_item,"}"),"{");
		} else {
			// if item did not have {} and is still numeric, don't search for value
			// it must be numeric constant
			if (is_numeric($search_item))
				$search_item = '';
		}
		
		if (array_key_exists($search_item, $calculated))
		{
			$item = $calculated[trim($search_item)];
			$item = empty($item) ? 0 : $item;
		}
	}
	$expression = implode(" ",$calc_array);
	$m = new EvalMath;
	$value = $m->evaluate($expression);
	//return empty($value) ? 0 : $value;
	return $value;
}

function calculate_account_total($from, $to, $accounts)
{
	$total = get_tax_account_balance($from, $to, $accounts);
	//return empty($total) ? 0 : $total;
	return $total;
}

function calculate_tax_report_line($basis, $from, $to, $tax_report_line, $calculated) 
{
	$accounts = get_tax_report_line_accounts($tax_report_line['tax_rep_line_id']);
	switch ($tax_report_line['line_type']) {
		case 'tax_code':
			return calculate_tax_code($basis, $from, $to, $tax_report_line['code_basis'], $accounts);
		case 'account_code':
			return calculate_account_total($from, $to, $accounts);
		case "compute":
			return calculate_computation($tax_report_line['calculation'], $calculated);
	}
	return "";
}

//----------------------------------------------------------------------------------------------------

function show_results()
{
	global $path_to_root;

    /*Now get the transactions  */
	div_start('trans_tbl');
	start_table(TABLESTYLE);

	$tax_report = get_tax_report($_POST['tax_rep_id']);
	$all_lines = get_tax_report_lines($_POST['tax_rep_id']);
	$calculated = array();
	$k = 0;
	while ($myrow = db_fetch($all_lines)) 
	{
		alt_table_row_color($k);
		label_cell($myrow['code']);
		$value = calculate_tax_report_line($tax_report['basis'], $_POST['TransFromDate'], $_POST['TransToDate'], $myrow, $calculated);
		$calculated[$myrow['code']] = $value;
		$prompt = $myrow['description'];
		switch ($myrow['line_type'])
		{
			case 'calculated':
				label_cells($prompt,$myrow['code'],price_format($value));
				break;
			case 'user':
				text_cells($prompt,$myrow['code'],$value);
				break;
			default:
				amount_cells($prompt,$myrow['code'],price_format($value), "align='left'");
				break;
		}
		end_row();
	}
	end_table(2);
	
	if ($tax_report['balancing_journal'])
	{
		$tax_accounts = get_tax_report_journal($_POST['TransFromDate'], $_POST['TransToDate']);
		start_table(TABLESTYLE);
	   	label_cell('<b>'._('Balancing Journal').'</b>','colspan=5 align=center');
		$th = array(_("Account Code"), _("Account Name"),
			_("Debit"), _("Credit"), _("Memo"));	
	    table_header($th);	
	   	$k = $payable = 0;
		while ($myrow = db_fetch($tax_accounts)) 
		{
			if ($myrow['payable'] != 0)
			{
				alt_table_row_color($k);
				label_cell($myrow['sales_gl_code']);
				label_cell($myrow['sales_name']);
				$value = $myrow['payable'];
				$payable += $value;
				amount_cells_ex(null, $value, 15, 20, price_format($value), "align='left'");
				label_cell("");
				label_cell("");
				end_row();
			}
			if ($myrow['collectible'] != 0)
			{
				alt_table_row_color($k);
				label_cell($myrow['purchasing_gl_code']);
				label_cell($myrow['purchasing_name']);
				label_cell("");
				$value = 0 - $myrow['collectible'];
				$payable -= $value;
				amount_cells_ex(null, $value, 15, 20, price_format($value), "align='left'");
				label_cell("");
				end_row();
			}
		}
		if ($payable != 0) {
			alt_table_row_color($k);
			label_cell($tax_report['liability_account']);
			label_cell($tax_report['liability_name']);
			label_cell("");
			amount_cells_ex(null, $payable, 15, 20, price_format($payable), "align='left'");
			label_cell("");
			end_row();
		}
		end_table(2);
	}
	div_end();
}

//----------------------------------------------------------------------------------------------------

tax_inquiry_controls();

show_results();

//----------------------------------------------------------------------------------------------------

end_page();

?>
