<?php
// ----------------------------------------------------------------
// $ Revision:  1.0 $
// Creator: Alastair Robertson
// date_:   2013-05-23
// Title:   UI functions for tax report
// Free software under GNU GPL
// ----------------------------------------------------------------
function down_button_cell($name, $value, $title=false)
{
	tax_report_button_cell($name, $value, $title, "down.png");
}
function up_button_cell($name, $value, $title=false)
{
	tax_report_button_cell($name, $value, $title, "up.png");
}

function tax_report_button_cell($name, $value, $title=false, $icon=false, $aspect='')
{
	echo "<td align='center'>";
	echo tax_report_button($name, $value, $title, $icon, $aspect);
	echo "</td>";
}

function tax_report_set_icon($icon, $title=false)
{
	global $path_to_root;
	if (basename($icon) === $icon) // standard icons does not contain path separator
		$icon = "$path_to_root/modules/rep_tax/images/$icon";
	return "<img src='$icon' width='12' height='12' border='0'".($title ? " title='$title'" : "")." />\n";	
}

function tax_report_button($name, $value, $title=false, $icon=false,  $aspect='')
{
	// php silently changes dots,spaces,'[' and characters 128-159
	// to underscore in POST names, to maintain compatibility with register_globals
	$rel = '';
	if ($aspect == 'selector') {
		$rel = " rel='$value'";
		$value = _("Select");
	}
	if (user_graphic_links() && $icon)
	{
		if ($value == _("Delete")) // Helper during implementation
			$icon = ICON_DELETE;
		return "<button type='submit' class='editbutton' name='"
			.htmlentities(strtr($name, array('.'=>'=2E', '='=>'=3D',// ' '=>'=20','['=>'=5B'
			)))
			."' value='1'" . ($title ? " title='$title'":" title='$value'")
			. ($aspect ? " aspect='$aspect'" : '')
			. $rel
			." />".tax_report_set_icon($icon)."</button>\n";
	}
	else
		return "<input type='submit' class='editbutton' name='"
			.htmlentities(strtr($name, array('.'=>'=2E', '='=>'=3D',// ' '=>'=20','['=>'=5B'
			)))
			."' value='$value'"
			.($title ? " title='$title'":'')
			. ($aspect ? " aspect='$aspect'" : '')
			. $rel
			." />\n";
}

function tax_report_list($name, $selected_id=null, $spec_option=false, $submit_on_change=false)
{
	global $all_items;

	$sql = "SELECT tax_rep_id, code, description FROM ".TB_PREF."tax_rep ";

	$ret = combo_input($name, $selected_id, $sql, 'tax_rep_id', 'code',
	array(
	    'format' => '_format_add_curr',
	    'order' => array('code'),
		'search_box' => false,
		'type' => 1,
		'size' => 20,
		'spec_option' => $spec_option === true ? _("All Tax Reports") : $spec_option,
		'spec_id' => $all_items,
		'select_submit'=> $submit_on_change,
		'async' => false,
		'sel_hint' => _('Select tax report'),
		'show_inactive' => false
	) );
	return $ret;
}

function tax_report_list_cells($label, $name, $selected_id=null, $all_option=false, 
	$submit_on_change=false, $show_inactive=false, $editkey = false)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td nowrap>";
	echo tax_report_list($name, $selected_id, $all_option, $submit_on_change,
		$show_inactive, $editkey);
	echo "</td>\n";
}

function tax_report_list_row($label, $name, $selected=null, $submit_on_change=false)
{
	echo "<tr><td class='label'>$label</td>";
	tax_report_list_cells(null, $name, $selected, $submit_on_change);
	echo "</tr>\n";
}

$basis_types = array( 'Accrual' => _("Accrual"),
			 		'Payment' => _("Payment"));

function report_basis_cells($label, $name, $selected=null, $submit_on_change=false)
{
	global $basis_types;
	if ($label != null)
		label_cell($label);
	echo "<td>\n";
	echo array_selector($name, $selected, $basis_types, 
		array( 
			'select_submit'=> $submit_on_change
		) );
	echo "</td>\n";
}

function report_basis_row($label, $name, $selected=null, $submit_on_change=false)
{
	echo "<tr><td class='label'>$label</td>";
	report_basis_cells(null, $name, $selected, $submit_on_change);
	echo "</tr>\n";
}

$code_bases = array( 'SalesGross' => _("Sales invoice total including tax"),
			 		'SalesNet' => _("Sales invoice total less tax"),
			 		'SalesTax' => _("Sales tax amount"),
					'PurchaseGross' => _("Purchases invoice total including tax"),
			 		'PurchaseNet' => _("Purchase invoice total less tax"),
			 		'PurchaseTax' => _("Purchases tax amount"));

function report_gross_cells($label, $name, $selected=null, $submit_on_change=false)
{
	global $code_bases;
	if ($label != null)
		label_cell($label);
	echo "<td>\n";
	echo array_selector($name, $selected, $code_bases, 
		array( 
			'select_submit'=> $submit_on_change
		) );
	echo "</td>\n";
}

function report_gross_row($label, $name, $selected=null, $submit_on_change=false)
{
	echo "<tr><td class='label'>$label</td>";
	report_gross_cells(null, $name, $selected, $submit_on_change);
	echo "</tr>\n";
}
$line_types = array( 'tax_code' => _("Tax code"),
			 		'account_code' => _("Account code"),
			 		'compute' => _("Computation"),
			 		'user' => _("User entry"));

function report_line_type_cells($label, $name, $selected=null, $submit_on_change=false)
{
	global $line_types;
	if ($label != null)
		label_cell($label);
	echo "<td>\n";
	echo array_selector($name, $selected, $line_types, 
		array( 
			'select_submit'=> $submit_on_change
		) );
	echo "</td>\n";
}

function report_line_type_row($label, $name, $selected=null, $submit_on_change=false)
{
	echo "<tr><td class='label'>$label</td>";
	report_line_type_cells(null, $name, $selected, $submit_on_change);
	echo "</tr>\n";
}

function multi_gl_all_accounts_list($name, $selected_id=null, $skip_bank_accounts=false,
	$cells=false, $all_option=false, $submit_on_change=false, $all=false, $height=10)
{
	if ($skip_bank_accounts)
		$sql = "SELECT chart.account_code, chart.account_name, type.name, chart.inactive, type.id
			FROM (".TB_PREF."chart_master chart,".TB_PREF."chart_types type) "
			."LEFT JOIN ".TB_PREF."bank_accounts acc "
			."ON chart.account_code=acc.account_code
				WHERE acc.account_code  IS NULL
			AND chart.account_type=type.id";
	else
		$sql = "SELECT chart.account_code, chart.account_name, type.name, chart.inactive, type.id
			FROM ".TB_PREF."chart_master chart,".TB_PREF."chart_types type
			WHERE chart.account_type=type.id";

	return combo_input($name, $selected_id, $sql, 'chart.account_code', 'chart.account_name',
	array(
		'format' => '_format_account',
		'spec_option' => $all_option===true ?  _("Use Item Sales Accounts") : $all_option,
		'spec_id' => '',
		'type' => 2,
		'order' => array('type.class_id','type.id','account_code'),
		'search_box' => $cells,
			'search_submit' => false,
			'size' => 12,
			'max' => 10,
			'cells' => true,
		'multi' => true,
		'height' => $height,
		'select_submit'=> $submit_on_change,
		'async' => false,
		'category' => 2,
		'show_inactive' => $all
	) );

}

function multi_gl_all_accounts_list_cells($label, $name, $selected_id=null, 
	$skip_bank_accounts=false, $cells=false, $all_option=false, 
	$submit_on_change=false, $all=false, $height=10)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	echo multi_gl_all_accounts_list($name, $selected_id, 
		$skip_bank_accounts, $cells, $all_option, $submit_on_change, $all, $height);
	echo "</td>\n";
}

function multi_gl_all_accounts_list_row($label, $name, $selected_id=null, 
	$skip_bank_accounts=false, $cells=false, $all_option=false, $height=10)
{
	echo "<tr><td class='label'>$label</td>";
	multi_gl_all_accounts_list_cells(null, $name, $selected_id, 
		$skip_bank_accounts, $cells, $all_option, $height);
	echo "</tr>\n";
}

function multi_tax_types_list($name, $selected_id=null, $none_option=false, $submit_on_change=false, $height=10)
{
	$sql = "SELECT id, CONCAT(name, ' (',rate,'%)') as name FROM ".TB_PREF."tax_types";

	return combo_input($name, $selected_id, $sql, 'id', 'name',
	array(
		'spec_option' => $none_option,
		'spec_id' => ALL_NUMERIC,
		'select_submit'=> $submit_on_change,
		'async' => false,
		'multi' => true,
		'height' => $height
	) );
}

function multi_tax_types_list_cells($label, $name, $selected_id=null, $none_option=false,
	$submit_on_change=false, $height=10)
{
	if ($label != null)
		echo "<td>$label</td>\n";
	echo "<td>";
	echo multi_tax_types_list($name, $selected_id, $none_option, $submit_on_change, $height);
	echo "</td>\n";
}

function multi_tax_types_list_row($label, $name, $selected_id=null, $none_option=false,
	$submit_on_change=false, $height=10)
{
	echo "<tr><td class='label'>$label</td>";
	multi_tax_types_list_cells(null, $name, $selected_id, $none_option, $submit_on_change, $height);
	echo "</tr>\n";
}

?>