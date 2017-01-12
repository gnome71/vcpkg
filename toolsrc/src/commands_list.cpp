#include "vcpkg_Commands.h"
#include "vcpkg.h"
#include "vcpkg_System.h"
#include "vcpkglib_helpers.h"
#include "Paragraphs.h"

namespace vcpkg
{
	static const std::string OPTION_DISKUSAGE = "--diskusage";
	
	static void do_print(const StatusParagraph& pgh, bool showDiskUsage)
    {
		if (!showDiskUsage) {
			System::println("%-27s %-16s %s",
				pgh.package.displayname(),
				pgh.package.version,
				details::shorten_description(pgh.package.description));
		}
		else
		{
			System::println("%-27s %-16s %s %s",
				pgh.package.displayname(),
				pgh.package.version,
				pgh.package.diskusage,	// CONTINUE
				details::shorten_description(pgh.package.description));
		}
    }

    void list_command(const vcpkg_cmd_arguments& args, const vcpkg_paths& paths)
    {
        static const std::string example = Strings::format(
            "The argument should be a substring to search for, or no argument to display all installed libraries.\n%s", create_example_string("list png"));
        args.check_max_arg_count(1, example);

		const std::unordered_set<std::string> options = args.check_and_get_optional_command_arguments({ OPTION_DISKUSAGE });
		bool alsoShowDiskUsage = options.find(OPTION_DISKUSAGE) != options.end();

        const StatusParagraphs status_paragraphs = database_load_check(paths);
        std::vector<StatusParagraph> installed_packages;
        for (auto&& pgh : status_paragraphs)
        {
            if (pgh->state == install_state_t::not_installed && pgh->want == want_t::purge)
                continue;
            installed_packages.push_back(*pgh);
        }

        if (installed_packages.empty())
        {
            System::println("No packages are installed. Did you mean `search`?");
            exit(EXIT_SUCCESS);
        }

        std::sort(installed_packages.begin(), installed_packages.end(),
                  [ ]( const StatusParagraph& lhs, const StatusParagraph& rhs ) -> bool
                  {
                      return lhs.package.displayname() < rhs.package.displayname();
                  });

		if (args.command_arguments.size() == 0)
        {
            for (const StatusParagraph& status_paragraph : installed_packages)
            {
                do_print(status_paragraph, alsoShowDiskUsage);
            }
        }
        else
        {
            // At this point there is 1 argument
            for (const StatusParagraph& status_paragraph : installed_packages)
            {
                const std::string displayname = status_paragraph.package.displayname();
                if (Strings::case_insensitive_ascii_find(displayname, args.command_arguments[0]) == displayname.end())
                {
                    continue;
                }

                do_print(status_paragraph, alsoShowDiskUsage);
            }
        }

        exit(EXIT_SUCCESS);
    }
}
