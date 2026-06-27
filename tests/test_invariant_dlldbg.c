#include <check.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

/* Forward declaration of the actual function from the production code */
extern LRESULT CALLBACK DbgDlgProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

START_TEST(test_sprintf_buffer_bounds)
{
    /* Invariant: WM_QUERYENDSESSION message handling must not overflow fmtBuffer */
    const char *payloads[] = {
        "A",  /* Minimal valid input */
        "This is a normal DLL name",  /* Typical valid input */
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"  /* 256 chars - boundary */
    };
    int num_payloads = sizeof(payloads) / sizeof(payloads[0]);

    for (int i = 0; i < num_payloads; i++) {
        /* Create a test window */
        HWND hwnd = CreateWindowEx(0, "STATIC", "Test", WS_OVERLAPPED,
                                   0, 0, 100, 100, NULL, NULL, NULL, NULL);
        
        /* Simulate the vulnerable code path with adversarial input */
        LRESULT result = DbgDlgProc(hwnd, WM_QUERYENDSESSION, 0, (LPARAM)payloads[i]);
        
        /* Property: The function must return a valid LRESULT without crashing */
        ck_assert_msg(result == 0 || result == 1, 
                     "Function returned invalid result for payload %d", i);
        
        /* Additional check: Ensure no buffer overflow corrupted critical memory */
        ck_assert_msg(GetLastError() == 0 || GetLastError() == ERROR_SUCCESS,
                     "System error detected after processing payload %d", i);
        
        DestroyWindow(hwnd);
    }
}
END_TEST

Suite *security_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("Security");
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_sprintf_buffer_bounds);
    suite_add_tcase(s, tc_core);

    return s;
}

int main(void)
{
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = security_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}