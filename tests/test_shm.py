import unittest

import shmdict

from tests.utils import IPCS

class SharedMemoryTest(unittest.TestCase):

    def setUp(self):
        self.segments = sorted(IPCS.list(), key=lambda s: s["key"])

    def tearDown(self):
        segments = sorted(IPCS.list(), key=lambda s: s["key"])
        self.assertEqual(segments, self.segments)

    def test_attach_with_wrong_types(self):
        self.assertRaises(ValueError, shmdict.attach, None, 1)
        self.assertRaises(ValueError, shmdict.attach, "string", None)

    def test_attach_detach_delete(self):
        seg = shmdict.attach("segment_name", 1024)
        self.assertTrue(seg)

        shmdict.detach(seg)
        shmdict.delete(seg)

    def test_attach_delete(self):
        seg = shmdict.attach("segment_name", 1024)
        self.assertRaises(ValueError, shmdict.delete, seg)

        # clean up
        shmdict.detach(seg)
        shmdict.delete(seg)

    def test_detach(self):
        seg = shmdict.attach("segment_name", 1024)
        shmdict.detach(seg)

        self.assertRaises(ValueError, shmdict.detach, seg)

        # clean up
        shmdict.delete(seg)

if __name__ == "__main__":
    unittest.main()
