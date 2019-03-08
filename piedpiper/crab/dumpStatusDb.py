#!/usr/bin/env python
import os
import sqlite3


def main():

    JOB_STATUS_DB = os.path.join(
        os.path.dirname(os.path.abspath(__file__)), 'crabjobsStatus.sqlite')

    conn = sqlite3.connect(JOB_STATUS_DB)
    with conn:
        c = conn.cursor()
        for row in c.execute(
                "SELECT * FROM crabJobStatuses WHERE status='completed'"):
            print('[ {0:^10} ]\t{1}'.format(row[1], row[2] or row[0]))
        for row in c.execute(
                "SELECT * FROM crabJobStatuses WHERE status='failed'"):
            print('[ {0:^10} ]\t{1}'.format(row[1], row[0]))


if __name__ == "__main__":
    main()
