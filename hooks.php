<?php
// ----------------------------------------------------------------
// $ Revision:  1.0 $
// Creator: Alastair Robertson
// date_:   2013-05-20
// Free software under GNU GPL
// Title:   Hooks for tax report
// Free software under GNU GPL
// ----------------------------------------------------------------

define ('SS_TAXREPEX', 103<<8);
class hooks_rep_tax extends hooks {
	var $module_name = 'Tax Reporting'; 

	/*
		Install additonal menu options provided by module
	 */
	function install_options($app) {
		global $path_to_root;

		switch($app->id) {
		case 'GL':
			$app->add_lapp_function(1, _('Tax Report'), 
					$path_to_root.'/modules/rep_tax/tax_report.php', 'SA_TAXREPEX');
			break;
		case 'system':
			$app->add_lapp_function(1, _('Tax Report Setup'), 
				$path_to_root.'/modules/rep_tax/tax_report_setup.php', 'SA_TAXREPEXSETUP', MENU_MAINTENANCE);
			break;
		}
	}

	function install_access()
	{
		$security_sections[SS_TAXREPEX] =	_("Tax Report");

		$security_areas['SA_TAXREPEX'] = array(SS_TAXREPEX|100, _("Tax Report"));
		$security_areas['SA_TAXREPEXSETUP'] = array(SS_TAXREPEX|101, _("Setup Tax Report"));

		return array($security_areas, $security_sections);
	}

		/* This method is called on extension activation for company.   */
	function activate_extension($company, $check_only=true) {
		global $db_connections;
		$updates = array(
			'update.sql' => array('tax_rep')
		);
		return $this->update_databases($company, $updates, $check_only);
	}
}
?>
