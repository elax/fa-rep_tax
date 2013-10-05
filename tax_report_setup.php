<?php
/**********************************************************************
    Copyright (C) FrontAccounting, LLC.
	Released under the terms of the GNU General Public License, GPL, 
	as published by the Free Software Foundation, either version 3 
	of the License, or (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
    See the License here <http://www.gnu.org/licenses/gpl-3.0.html>.
***********************************************************************/
$page_security = 'SA_TAXREPSETUP';
$path_to_root = "../..";
include($path_to_root . "/includes/session.inc");
add_access_extensions();

page(_($help_context = "Tax Reports"));

include($path_to_root . "/modules/rep_tax/includes/tax_report_db.inc");
include_once($path_to_root . "/modules/rep_tax/includes/tax_report_ui.inc");

include($path_to_root . "/includes/ui.inc");

global $code_bases, $line_types;

simple_page_mode(true);
simple_page_mode2(true);

function simple_page_mode2($numeric_id = true)
{
	global $Ajax, $Mode2, $selected_id2;

	$default = $numeric_id ? -1 : '';
	$selected_id2 = get_post('selected_id2', $default);
	foreach (array('ADD_ITEM2', 'UPDATE_ITEM2', 'RESET2') as $m) {
		if (isset($_POST[$m])) {
			$Ajax->activate('_page_body');
			if ($m == 'RESET2') 
				$selected_id2 = $default;
			$Mode2 = $m; return;
		}
	}
	foreach (array('BEd', 'BDel', 'BDwn', 'BUp') as $m) {
		foreach ($_POST as $p => $pvar) {
			if (strpos($p, $m) === 0) {
				unset($_POST['_focus']); // focus on first form entry
				$selected_id2 = quoted_printable_decode(substr($p, strlen($m)));
				$Ajax->activate('_page_body');
				$Mode2 = $m;
				return;
			}
		}
	}
	$Mode2 = '';
}

function submit_add_or_update_center2($add=true, $title=false, $async=false)
{
	echo "<center>";
	if ($add)
		submit('ADD_ITEM2', _("Add new"), true, $title, $async);
	else {
		submit('UPDATE_ITEM2', _("Update"), true, $title, $async);
		submit('RESET2', _("Cancel"), true, $title, $async);
	}
	echo "</center>";
}

//-----------------------------------------------------------------------------------

function can_process() 
{

	if (strlen($_POST['description']) == 0) 
	{
		display_error( _("The Tax Report description cannot be empty."));
		set_focus('description');
		return false;
	}

	return true;
}

function can_process_line() 
{

	if (strlen($_POST['line_code']) == 0) 
	{
		display_error( _("The report line code cannot be empty."));
		set_focus('line_code');
		return false;
	}
		if (strlen($_POST['line_description']) == 0) 
	{
		display_error( _("The report line description cannot be empty."));
		set_focus('line_description');
		return false;
	}
	
	return true;
}

//-----------------------------------------------------------------------------------

if ($Mode=='ADD_ITEM' || $Mode=='UPDATE_ITEM') 
{

	if (can_process()) 	
	{	

		if ($selected_id != -1) 
		{
			update_tax_rep($selected_id, $_POST['code'], $_POST['description'], $_POST['basis'],
				 $_POST['liability_account'], $_POST['bank_account'], check_value('balancing_journal'), check_value('tax_payment'));
			display_notification(_('Selected Tax Report has been updated'));
		} 
		else 
		{
			add_tax_rep($_POST['code'], $_POST['description'], $_POST['basis'], $_POST['liability_account'], 
				$_POST['bank_account'], check_value('balancing_journal'), check_value('tax_payment'));
			display_notification(_('New Tax Report has been added'));
		}
		$Mode = 'RESET';
	}
}

if ($Mode2=='ADD_ITEM2' || $Mode2=='UPDATE_ITEM2') 
{
	if (can_process_line()) 	
	{	
		if ($selected_id2 != -1) 
		{
			update_tax_report_line($selected_id2, $selected_id, $_POST['line_code'], $_POST['line_description'], $_POST['line_type'], $_POST['calculation'], $_POST['code_basis'], $_POST['accounts']);
			display_notification(_('Selected Tax Report line has been updated'));
		} 
		else 
		{
			add_tax_report_line($selected_id, $_POST['line_code'], $_POST['line_description'], $_POST['line_type'], $_POST['calculation'], $_POST['code_basis'], $_POST['accounts']);
			display_notification(_('New Tax Report line has been added'));
		}
		$Mode2 = 'RESET2';
	}
}

if ($Mode2=='BDwn' || $Mode2=='BUp') 
{
	if ($selected_id2 != -1) 
	{
		update_tax_report_line_sequence($selected_id2, $Mode2 == "BDwn" ? "down" : "up" );
	} 
	$Mode2 = 'RESET2';
}

//-----------------------------------------------------------------------------------

if ($Mode == 'Delete')
{
	if (!db_has_tax_report_lines($selected_id))
	{
		delete_tax_report($selected_id);
		display_notification(_('Selected Tax Report has been deleted'));
		$Mode = 'RESET';
	}
	else
	{
		display_error( _("The Tax Report has Tax Report Lines. Cannot be deleted."));
		set_focus('code');
	}
}

if (find_submit('Edit') != -1) {
	$Mode2 = 'RESET2';
	set_focus('code');
}
if (find_submit('BEd') != -1 || get_post('ADD_ITEM2')) {
	set_focus('line_code');
}

if ($Mode2 == 'BDel')
{
	delete_tax_report_line($selected_id2);
	display_notification(_('Selected Tax Report line has been deleted'));
	$Mode2 = 'RESET2';
}
//-----------------------------------------------------------------------------------
if ($Mode == 'RESET')
{
	$selected_id = -1;
	$_POST['description'] = $_POST['code'] = '';
	$_POST['basis']= 'accrual';
	$_POST['liability_account'] = $_POST['bank_account'] = '';
	unset($_POST['balancing_journal']);
	unset($_POST['tax_payment']);
}
if ($Mode2 == 'RESET2')
{
	$selected_id2 = -1;
	$_POST['line_code'] = $_POST['line_description'] = $_POST['line_type'] = 
		$_POST['calculation'] = $_POST['code_basis']= '';
}
//-----------------------------------------------------------------------------------

$result = get_tax_reports(check_value('show_inactive'));
start_form();
start_table(TABLESTYLE);
$th = array("",_("Tax Report"), "", "");
inactive_control_column($th);
table_header($th);

$k = 0;
while ($myrow = db_fetch($result)) 
{
	alt_table_row_color($k);
	label_cell($myrow['code']);
	label_cell($myrow['description']);
	inactive_control_cell($myrow["tax_rep_id"], $myrow["inactive"], 'tax_rep', 'tax_rep_id');
	edit_button_cell("Edit".$myrow["tax_rep_id"], _("Edit"));
	delete_button_cell("Delete".$myrow["tax_rep_id"], _("Delete"));
	end_row();
}
inactive_control_row($th);
end_table(1);
//-----------------------------------------------------------------------------------

div_start('tr');
start_table(TABLESTYLE2);

if ($selected_id != -1) 
{
	if ($Mode == 'Edit') 
	{
		unset($_POST['balancing_journal']);
		unset($_POST['tax_payment']);
		$myrow = get_tax_report($selected_id);

		$_POST['tax_rep_id']  = $myrow["tax_rep_id"];
		$_POST['code']  = $myrow["code"];
		$_POST['description']  = $myrow["description"];
		$_POST['basis']  = $myrow["basis"];
		$_POST['liability_account']  = $myrow["liability_account"];
		$_POST['bank_account']  = $myrow["bank_account"];
		if ($myrow["balancing_journal"]) $_POST['balancing_journal'] = 1;
		if ($myrow["tax_payment"]) $_POST['tax_payment'] = 1;
	}	
	hidden('selected_id', $selected_id);
} 

if (list_updated('balancing_journal') || list_updated('tax_payment'))
{
	$Ajax->activate('tr');
}

text_row_ex(_("Code").":", 'code', 20, 20);
text_row_ex(_("Description").':', 'description', 50, 100);
report_basis_row(_("Report Basis").":", 'basis', null, true);
check_row(_("Create Balancing Journal").":", 'balancing_journal', check_value('balancing_journal'), true);
if (isset($_POST['balancing_journal']) && $_POST['balancing_journal'])
	gl_all_accounts_list_row(_("Tax Liability Account:"), 'liability_account', null, true);
else 
	hidden('liability_account', '');

check_row(_("Create Tax Payment").":", 'tax_payment', check_value('tax_payment'), true);
if (isset($_POST['tax_payment']) && $_POST['tax_payment'])
	bank_accounts_list_row(_("Bank Account:"), 'bank_account', null, true);
else 
	hidden('bank_account', '');
	
end_table(1);
submit_add_or_update_center($selected_id == -1, '', 'both');
div_end();


if ($selected_id != -1)
{
	br();
	display_heading(_("Tax Report Lines") . " - " . $_POST['description']);
	$result = get_tax_report_lines($selected_id);

	start_table(TABLESTYLE2);
	$th = array(_("Code"), _("Description"), _("Process"), "", "", "", "");

	table_header($th);
	$k = $rec = 0;
	$maxrec = get_tax_report_lines_count($selected_id);
	while ($myrow = db_fetch($result)) 
	{
		$rec++;
		alt_table_row_color($k);
		
		label_cell($myrow['code']);
		label_cell($myrow['description']);
		switch ($myrow['line_type']) {
			case 'tax_code':
				$proc = $code_bases[$myrow['code_basis']];
				break;
			case 'account_code':
				$proc = _("Sum of accounts totals");
				break;
			case "compute":
				$proc = $myrow['calculation'];
				break;
			case 'user':
				$proc = '';
				break;
		}
		label_cell($proc);

		if ($rec < $maxrec)
			down_button_cell("BDwn".$myrow["tax_rep_line_id"], _("Down"));
		else
			echo '<td/>';
		if ($rec > 1)
			up_button_cell("BUp".$myrow["tax_rep_line_id"], _("Up"));
		else
			echo '<td/>';
		edit_button_cell("BEd".$myrow["tax_rep_line_id"], _("Edit"));
		delete_button_cell("BDel".$myrow["tax_rep_line_id"], _("Delete"));
		end_row();
	}
	end_table(1);

	div_start('edit_line');
	start_table(TABLESTYLE2);

	if ($selected_id2 != -1) 
	{
	 	if ($Mode2 == 'BEd') 
	 	{
			//editing an existing status code
			$myrow = get_tax_report_line($selected_id2);

			$_POST['tax_rep_line_id']  = $myrow["tax_rep_line_id"];
			$_POST['line_code']  = $myrow["code"];
			$_POST['line_description']  = $myrow["description"];
			$_POST['line_type']  = $myrow["line_type"];
			$_POST['code_basis']  = $myrow["code_basis"];
			$_POST['calculation']  = $myrow["calculation"];
			
			$_POST['accounts']  = get_tax_report_line_accounts($selected_id2);
	 	}
	} 

	text_row_ex(_("Code").":", 'line_code', 20, 20);
	text_row_ex(_("Description").':', 'line_description', 50, 100);
	report_line_type_row(_("Line Type").":", 'line_type', null, true);
	
	if (list_updated('line_type'))
		$Ajax->activate('edit_line');

	$actn = strtolower($_POST['line_type']);

	switch ($actn) {
		case 'tax_code':
			report_gross_row(_("Basis").":", 'code_basis', null, false);
			multi_tax_types_list_row(_("Tax Type").":", 'accounts', null, false, false, 5);
			hidden('calculation', '');
			break; 
		case 'account_code':
			multi_gl_all_accounts_list_row(_("Account").":", 'accounts', null, true);
			hidden('calculation', '');
			hidden('code_basis', '');
			break; 
		case 'compute':
			text_row_ex(_("Calculation").':', 'calculation', 50, 100);
			hidden('code_basis', '');
			hidden('accounts', '');
			break; 
		case 'user':
			hidden('code_basis', '');
			hidden('accounts', '');
			hidden('calculation', '');
			break;
	}
	
	end_table(1);
	div_end();
	
	hidden('selected_id', $selected_id);
	hidden('selected_id2', $selected_id2);

	submit_add_or_update_center2($selected_id2 == -1, '', true);

}		
end_form();
//------------------------------------------------------------------------------------

end_page();

?>