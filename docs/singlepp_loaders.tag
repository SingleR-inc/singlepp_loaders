<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>
<tagfile doxygen_version="1.9.8">
  <compound kind="file">
    <name>labels.hpp</name>
    <path>singlepp_loaders/</path>
    <filename>labels_8hpp.html</filename>
    <class kind="struct">singlepp_loaders::LoadLabelsOptions</class>
    <namespace>singlepp_loaders</namespace>
  </compound>
  <compound kind="file">
    <name>markers.hpp</name>
    <path>singlepp_loaders/</path>
    <filename>markers_8hpp.html</filename>
    <class kind="struct">singlepp_loaders::LoadMarkersOptions</class>
    <namespace>singlepp_loaders</namespace>
  </compound>
  <compound kind="file">
    <name>rankings.hpp</name>
    <path>singlepp_loaders/</path>
    <filename>rankings_8hpp.html</filename>
    <class kind="struct">singlepp_loaders::LoadRankingsOptions</class>
    <namespace>singlepp_loaders</namespace>
  </compound>
  <compound kind="file">
    <name>singlepp_loaders.hpp</name>
    <path>singlepp_loaders/</path>
    <filename>singlepp__loaders_8hpp.html</filename>
    <includes id="rankings_8hpp" name="rankings.hpp" local="yes" import="no" module="no" objc="no">rankings.hpp</includes>
    <includes id="markers_8hpp" name="markers.hpp" local="yes" import="no" module="no" objc="no">markers.hpp</includes>
    <includes id="labels_8hpp" name="labels.hpp" local="yes" import="no" module="no" objc="no">labels.hpp</includes>
    <includes id="verify_8hpp" name="verify.hpp" local="yes" import="no" module="no" objc="no">verify.hpp</includes>
    <namespace>singlepp_loaders</namespace>
  </compound>
  <compound kind="file">
    <name>verify.hpp</name>
    <path>singlepp_loaders/</path>
    <filename>verify_8hpp.html</filename>
    <includes id="rankings_8hpp" name="rankings.hpp" local="yes" import="no" module="no" objc="no">rankings.hpp</includes>
    <namespace>singlepp_loaders</namespace>
  </compound>
  <compound kind="struct">
    <name>singlepp_loaders::LoadLabelsOptions</name>
    <filename>structsinglepp__loaders_1_1LoadLabelsOptions.html</filename>
    <member kind="variable">
      <type>bool</type>
      <name>parallel</name>
      <anchorfile>structsinglepp__loaders_1_1LoadLabelsOptions.html</anchorfile>
      <anchor>ad2b8deb7a0097822d072660d416906fb</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>buffer_size</name>
      <anchorfile>structsinglepp__loaders_1_1LoadLabelsOptions.html</anchorfile>
      <anchor>a349ba1860b174664450635ee5a0c45b0</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>singlepp_loaders::LoadMarkersOptions</name>
    <filename>structsinglepp__loaders_1_1LoadMarkersOptions.html</filename>
    <member kind="variable">
      <type>bool</type>
      <name>parallel</name>
      <anchorfile>structsinglepp__loaders_1_1LoadMarkersOptions.html</anchorfile>
      <anchor>a68b230e7d2b1507b01e886eb6b140fec</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>buffer_size</name>
      <anchorfile>structsinglepp__loaders_1_1LoadMarkersOptions.html</anchorfile>
      <anchor>aef34d1def8ffb3b4abcf23e6a733fd5b</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>singlepp_loaders::LoadRankingsOptions</name>
    <filename>structsinglepp__loaders_1_1LoadRankingsOptions.html</filename>
    <member kind="variable">
      <type>bool</type>
      <name>parallel</name>
      <anchorfile>structsinglepp__loaders_1_1LoadRankingsOptions.html</anchorfile>
      <anchor>a234d454515da7bc046fc1107ee0ebb0e</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>buffer_size</name>
      <anchorfile>structsinglepp__loaders_1_1LoadRankingsOptions.html</anchorfile>
      <anchor>a424708c87f98bd5cee0f6b2f41bed123</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>singlepp_loaders</name>
    <filename>namespacesinglepp__loaders.html</filename>
    <class kind="struct">singlepp_loaders::LoadLabelsOptions</class>
    <class kind="struct">singlepp_loaders::LoadMarkersOptions</class>
    <class kind="struct">singlepp_loaders::LoadRankingsOptions</class>
    <member kind="typedef">
      <type>tatami::DenseMatrix&lt; Value_, Index_, std::vector&lt; Rank_ &gt; &gt;</type>
      <name>RankMatrix</name>
      <anchorfile>namespacesinglepp__loaders.html</anchorfile>
      <anchor>a128454a38edec4d080e5af3d12420231</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; Label_ &gt;</type>
      <name>load_labels_from_text_file</name>
      <anchorfile>namespacesinglepp__loaders.html</anchorfile>
      <anchor>a34ae681d0ec01edbe1e1c43cf46c0e18</anchor>
      <arglist>(const char *path, const LoadLabelsOptions &amp;options)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; Label_ &gt;</type>
      <name>load_labels_from_gzip_file</name>
      <anchorfile>namespacesinglepp__loaders.html</anchorfile>
      <anchor>a06ac566f453ce7ef661f3eedbd88fad2</anchor>
      <arglist>(const char *path, const LoadLabelsOptions &amp;options)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; Label_ &gt;</type>
      <name>load_labels_from_zlib_buffer</name>
      <anchorfile>namespacesinglepp__loaders.html</anchorfile>
      <anchor>a513141cd3dd9ca8200586c0e42ba6778</anchor>
      <arglist>(const unsigned char *buffer, size_t len, const LoadLabelsOptions &amp;options)</arglist>
    </member>
    <member kind="function">
      <type>singlepp::Markers&lt; Index_ &gt;</type>
      <name>load_markers_from_text_file</name>
      <anchorfile>namespacesinglepp__loaders.html</anchorfile>
      <anchor>afaa88ff4c4390b460027a7a811fd3829</anchor>
      <arglist>(const char *path, const LoadMarkersOptions &amp;options)</arglist>
    </member>
    <member kind="function">
      <type>singlepp::Markers&lt; Index_ &gt;</type>
      <name>load_markers_from_gzip_file</name>
      <anchorfile>namespacesinglepp__loaders.html</anchorfile>
      <anchor>a4c08eabc84fe8b41646b09313612f245</anchor>
      <arglist>(const char *path, const LoadMarkersOptions &amp;options)</arglist>
    </member>
    <member kind="function">
      <type>singlepp::Markers&lt; Index_ &gt;</type>
      <name>load_markers_from_zlib_buffer</name>
      <anchorfile>namespacesinglepp__loaders.html</anchorfile>
      <anchor>ab0df75fd4efe76ac466bf80619aab803</anchor>
      <arglist>(const unsigned char *buffer, size_t len, const LoadMarkersOptions &amp;options)</arglist>
    </member>
    <member kind="function">
      <type>RankMatrix&lt; Value_, Index_ &gt;</type>
      <name>load_rankings_from_text_file</name>
      <anchorfile>namespacesinglepp__loaders.html</anchorfile>
      <anchor>a65262597567aea1a3ee5e2a613c569f6</anchor>
      <arglist>(const char *path, const LoadRankingsOptions &amp;options)</arglist>
    </member>
    <member kind="function">
      <type>RankMatrix&lt; Value_, Index_ &gt;</type>
      <name>load_rankings_from_gzip_file</name>
      <anchorfile>namespacesinglepp__loaders.html</anchorfile>
      <anchor>acaf04c34411db697f92365945643299a</anchor>
      <arglist>(const char *path, const LoadRankingsOptions &amp;options)</arglist>
    </member>
    <member kind="function">
      <type>RankMatrix&lt; Value_, Index_ &gt;</type>
      <name>load_rankings_from_zlib_buffer</name>
      <anchorfile>namespacesinglepp__loaders.html</anchorfile>
      <anchor>a28edb2b406ca5566cb078a60777a6f12</anchor>
      <arglist>(const unsigned char *buffer, size_t len, const LoadRankingsOptions &amp;options)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>verify</name>
      <anchorfile>namespacesinglepp__loaders.html</anchorfile>
      <anchor>ac70ef4d9cabad2e3f452c40b50e7f30f</anchor>
      <arglist>(const RankMatrix&lt; Value_, Index_, Rank_ &gt; &amp;rankings, const std::vector&lt; Label_ &gt; &amp;labels, const singlepp::Markers&lt; Index_ &gt; &amp;markers)</arglist>
    </member>
  </compound>
  <compound kind="page">
    <name>index</name>
    <title>Load reference datasets for SingleR</title>
    <filename>index.html</filename>
    <docanchor file="index.html">md__2github_2workspace_2README</docanchor>
  </compound>
</tagfile>
